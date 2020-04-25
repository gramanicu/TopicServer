/**
 * Copyright (c) 2020 Grama Nicolae
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#include "Messages.hpp"
#include "Utils.hpp"

namespace application {
class Subscriber {
   private:
    uint sockfd, server_port;
    fd_set read_fds, tmp_fds;
    sockaddr_in server_addr;
    std::string client_id;

    // The database that links topic names to their id's
    std::unordered_map<uint, std::string> topics;
    std::set<std::string> queuedTopics;

    /**
     * @brief Return the name of a topic
     * Will return " " if the id was not sent by the server
     * @param id The id of the topic
     * @return std::string The name of the topic
     */
    std::string get_topic_name(uint id) {
        auto it = topics.find(id);

        if (it == topics.end()) {
            return " ";
        } else {
            return it->second;
        }
    }

    /**
     * @brief Return the id of a topic
     * Will return -1 if the topic id was not sent by the server
     * @param name The name of the topic
     * @return int The id of the topic
     */
    int get_topic_id(const std::string& name) {
        auto it =
            std::find_if(topics.begin(), topics.end(),
                         [&name](auto&& pair) { return pair.second == name; });

        if (it == topics.end()) {
            return -1;
        }
        return it->first;
    }

    /**
     * @brief Clear the file descriptors
     */
    void clear_fds() {
        FD_ZERO(&read_fds);
        FD_ZERO(&tmp_fds);
    }

#pragma GCC push_options  // Disable optimisations for this function
#pragma GCC optimize("-O0")
    /**
     * @brief Connect to server and send client info
     */
    void init_connection() {
        // Connect to the server
        MUST(connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) == 0,
             "Couldn't connect to the server");

        // Set the file descriptors for the sockets
        FD_SET(sockfd, &read_fds);

        // Set the file descriptor for STDIN
        FD_SET(STDIN_FILENO, &read_fds);

        // Send client info
        tcp_message msg;
        bzero(&msg, TCP_MSG_SIZE);
        msg.type = tcp_msg_type::CONNECT;
        tcp_connect data;

        safe_cpy(data.name, client_id.c_str(), client_id.size());
        memcpy(msg.payload, &data, TCP_DATA_CONNECT);
        // Send the client info
        CERR(send(sockfd, &msg, TCP_DATA_CONNECT + 1, 0) < 0);
    }
#pragma GCC pop_options

    /**
     * @brief Read TCP messages received from the srver
     * Will return whether the program should close. (the server closed)
     * @return true Close the program
     * @return false Continue the program
     */
    bool read_tcp_message() {
        tcp_message msg;
        bzero(&msg, TCP_MSG_SIZE);

        ssize_t msg_size = recv(sockfd, &msg, sizeof(msg), 0);
        CERR(msg_size < 0);

        if (msg_size == 0) {
            // The server disconnected
            close(sockfd);
            return true;
        } else {
            switch (msg.type) {
                case tcp_msg_type::TOPIC_ID: {
                    // Store the id of the topic in the topics map
                    tcp_topic_id data;
                    bzero(&data, TCP_DATA_TOPICID);
                    memcpy(&data, msg.payload, TCP_DATA_TOPICID);
                    topics.insert(std::make_pair(data.id, data.topic));

                    // If it was requested by this process, and not sent
                    // because the user was previously subscribed to it
                    auto it = queuedTopics.find(data.topic);
                    if (it != queuedTopics.end()) {
                        queuedTopics.erase(it);

                        std::cout << "Subscribed " << data.topic << "\n";
                    }
                } break;
                case tcp_msg_type::CONFIRM_U: {
                    // The server confirmed we are unsubscribed from this topic
                    tcp_confirm_u data;
                    bzero(&data, TCP_DATA_CONFIRM_U);
                    memcpy(&data, msg.payload, TCP_DATA_CONFIRM_U);

                    std::cout << "Unsubscribed " << topics[data.topic] << "\n";
                    topics.erase(data.topic);
                } break;
                case tcp_msg_type::DATA: {
                    tcp_data data;
                    bzero(&data, TCP_DATA_DATA);
                    memcpy(&data, msg.payload, TCP_DATA_DATA);
                    std::cout << data.payload << "\n";
                } break;
                default:
                    break;
            }
        }
        return false;
    }

    /**
     * @brief Read input from stdin
     * Will return whether the program should close.
     * @return true Close the program
     * @return false Continue the program
     */
    bool read_input() {
        std::string command;
        std::cin >> command;

        if (command == "exit") {
            return true;
        } else if (command == "subscribe") {
            // Subscribe
            std::string topic;
            bool sf;
            // If the second argument is not a number, the value will be false
            // If the second argument is greater than 1, the value will be true
            std::cin >> topic >> sf;

            // Send the subscribe request
            tcp_message msg;
            bzero(&msg, sizeof(msg));

            tcp_subscribe data;
            data.sf = sf;
            safe_cpy(data.topic, topic.c_str(), topic.size());

            msg.type = tcp_msg_type::SUBSCRIBE;
            memcpy(msg.payload, &data, TCP_DATA_SUBSCRIBE);

            // Send the client info
            CERR(send(sockfd, &msg, TCP_DATA_SUBSCRIBE + 1, 0) < 0);

            // Mark this topic as "requested by the client, waiting id"
            queuedTopics.insert(data.topic);
        } else if (command == "unsubscribe") {
            // Unsubscribe
            std::string topic;
            std::cin >> topic;

            // Send client info
            tcp_message msg;
            msg.type = tcp_msg_type::UNSUBSCRIBE;
            tcp_unsubscribe data;

            int id = get_topic_id(topic);

            // If the topic was actually subscribed to
            if (id != -1) {
                data.topic = id;
                memcpy(msg.payload, &data, TCP_DATA_UNSUBSCRIBE);
                // Send the client info
                CERR(send(sockfd, &msg, TCP_DATA_UNSUBSCRIBE, 0) < 0);
            }
        }
        return false;
    }

   public:
    /**
     * @brief Contrustor, initialises connections, etc.
     * @param id The id/name of the subscriber
     * @param ip The ip of the server
     * @param port The port of the server
     */
    Subscriber(const std::string& id, const char* ip, const uint port)
        : server_port(port), client_id(id) {
        // Initialise the TCP socket
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        MUST(sock >= 0, "Failed to initialise socket\n");
        sockfd = sock;

        // Clear the file descriptors sets
        clear_fds();

        // Set the socket options
        const int opt = 1;
        MUST(
            setsockopt(sockfd, SOL_SOCKET, TCP_NODELAY, &opt, sizeof(opt) == 0),
            "Couldn't set socket options\n");

        // Set the server adress
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port);
        MUST(inet_aton(ip, &server_addr.sin_addr) != 0, "Invalid IP adress\n");
    }

    /**
     * @brief Desctructor, to cleanup memory, etc.
     */
    ~Subscriber() {
        // Close connection
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
    }

    /**
     * @brief Starts the subscriber program
     * Will connect to the server and start sending/receiving messages, etc.
     */
    void run() {
        init_connection();

        do {
            tmp_fds = read_fds;

            // The maximum fd is the socket fd
            CERR(select(sockfd + 1, &tmp_fds, NULL, NULL, NULL) < 0);
            for (uint i = 0; i <= sockfd; ++i) {
                if (FD_ISSET(i, &tmp_fds)) {
                    if (i == STDIN_FILENO) {
                        if (read_input()) {
                            // Close the subscriber
                            return;
                        }
                    } else if (i == sockfd) {
                        if (read_tcp_message()) {
                            // Close the subscriber
                            return;
                        }
                    }
                }
            }
        }
        FOREVER;
    }
};
}  // namespace application