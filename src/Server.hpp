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

#include "Database.hpp"
#include "Messages.hpp"
#include "User.hpp"
#include "Utils.hpp"

namespace application {
class Server {
   private:
    uint main_port, main_tcp_sock, udp_sock, max_fd;
    fd_set read_fds, tmp_fds;
    sockaddr_in listen_addr;

    // The database that links topic names to their id's
    std::unordered_map<uint, std::string> topics;
    uint max_topic_id;

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
    int get_topic_id(const std::string &name) {
        auto it =
            std::find_if(topics.begin(), topics.end(),
                         [&name](auto &&pair) { return pair.second == name; });

        if (it == topics.end()) {
            return -1;
        }
        return it->first;
    }

    /**
     * @brief Add a new topic to the list
     * @param name The name of the topic
     * The id is automatically assigned
     */
    void add_topic(const std::string &name) {
        // If the topic doesn't exist already
        if (get_topic_id(name) == -1) {
            topics.insert(std::make_pair(max_topic_id, name));
            max_topic_id++;
        }
    }

    /**
     * @brief Clear the file descriptors
     */
    void clear_fds() {
        FD_ZERO(&read_fds);
        FD_ZERO(&tmp_fds);
    }

    /**
     * @brief Prepare, bind and start listening
     */
    void init_connections() {
        MUST(bind(main_tcp_sock, (sockaddr *)&listen_addr, sizeof(sockaddr)) >=
                 0,
             "Could not bind tcp socket\n");
        MUST(listen(main_tcp_sock, MAX_CLIENTS) >= 0,
             "Could not start listening for tcp connections\n");
        MUST(bind(udp_sock, (sockaddr *)&listen_addr, sizeof(sockaddr)) >= 0,
             "Could not bind udp socket\n");

        // Set the file descriptors for the sockets
        FD_SET(main_tcp_sock, &read_fds);
        FD_SET(udp_sock, &read_fds);
        max_fd = std::max(main_tcp_sock, udp_sock);

        // Set the file descriptor for STDIN
        FD_SET(STDIN_FILENO, &read_fds);
    }

    /**
     * @brief Read and execute commands from STDIN
     * Will return whether the program should close.
     * @return true Close the program
     * @return false Continue the program
     */
    bool read_input() {
        std::string command;
        std::cin >> command;

        if (command == "exit") {
            // TODO - Close all clients
            return true;
        }
        return false;
    }

    void process_udp_message() {}

    void read_udp_message() {
        udp_header hdr;
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        char buffer[UDP_MSG_SIZE];
        bzero(buffer, UDP_MSG_SIZE);

        int udp_msg_size = recvfrom(udp_sock, buffer, UDP_MSG_SIZE, 0,
                                    (sockaddr *)&client_addr, &client_len);

        std::cout << inet_ntoa(client_addr.sin_addr) << ":";
        std::cout << ntohs(client_addr.sin_port) << " - ";

        if (udp_msg_size > 0) {
            memcpy(&hdr, buffer, UDP_HDR_SIZE);
            std::cout << hdr.print() << " - ";
            add_topic(hdr.topic);
            switch (hdr.type) {
                case udp_msg_type::INT: {
                    udp_int_msg msg;
                    bzero(&msg, UDP_INT_SIZE);
                    memcpy(&msg, buffer, UDP_INT_SIZE);
                    std::cout << msg.print() << "\n";
                } break;
                case udp_msg_type::SHORT_REAL: {
                    udp_real_msg msg;
                    bzero(&msg, UDP_REAL_SIZE);
                    memcpy(&msg, buffer, UDP_REAL_SIZE);
                    std::cout << msg.print() << "\n";
                } break;
                case udp_msg_type::FLOAT: {
                    udp_float_msg msg;
                    bzero(&msg, UDP_FLOAT_SIZE);
                    memcpy(&msg, buffer, UDP_FLOAT_SIZE);
                    std::cout << msg.print() << "\n";
                } break;
                case udp_msg_type::STRING: {
                    udp_string_msg msg;
                    bzero(&msg, UDP_FLOAT_SIZE);
                    memcpy(&msg, buffer, udp_msg_size);
                    std::cout << msg.print() << "\n";
                }
            }
        }
    }

    void read_tcp_message(uint sockfd) {
        tcp_message msg;
        bzero(&msg, TCP_MSG_SIZE);

        ssize_t msg_size = recv(sockfd, &msg, sizeof(msg), 0);
        CERR(msg_size < 0);

        if (msg_size == 0) {
            // Client disconnected
            close(sockfd);
        } else {
            switch (msg.type) {
                case tcp_msg_type::CONNECT: {
                    tcp_connect data;
                    bzero(&data, TCP_DATA_CONNECT);
                    memcpy(&data, msg.payload, TCP_DATA_CONNECT);
                    std::cout << "New client " << data.name
                              << " connected from "
                              << "IP"
                              << ":"
                              << "PORT"
                              << ".\n";
                } break;
                case tcp_msg_type::SUBSCRIBE: {
                    tcp_subscribe data;
                    bzero(&data, TCP_DATA_SUBSCRIBE);
                    memcpy(&data, msg.payload, TCP_DATA_SUBSCRIBE);

                    if (data.sf) {
                        std::cout << "Subscribe " << data.topic << " FALSE\n";
                    } else {
                        std::cout << "Subscribe " << data.topic << " TRUE\n";
                    }

                    // Add the topic if it doesn't exist already
                    add_topic(data.topic);

                    // Send the id of the topic to the client
                    send_topic_id(sockfd, data.topic);
                } break;
                case tcp_msg_type::UNSUBSCRIBE: {
                    tcp_unsubscribe data;
                    bzero(&data, TCP_DATA_UNSUBSCRIBE);
                    memcpy(&data, msg.payload, TCP_DATA_UNSUBSCRIBE);

                    std::cout << "Unsubscribe " << data.topic << "\n";
                } break;
                default:
                    break;
            }
        }
    }

    /**
     * @brief Send the id of a topic to the client connected to the specified
     * sockfd
     * @param sockfd The socket file descriptor
     * @param name The name of the topic
     */
    void send_topic_id(const uint sockfd, const std::string &name) {
        tcp_message msg;
        bzero(&msg, TCP_MSG_SIZE);

        int id = get_topic_id(name);
        if (id != -1) {
            tcp_topic_id data;
            safe_cpy(data.topic, name.c_str(), name.size());
            data.id = id;
            std::cout << data.id << "\n";
            std::cout << data.topic << "\n";

            msg.type = tcp_msg_type::TOPIC_ID;
            memcpy(msg.payload, &data, TCP_DATA_TOPICID);

            // Send the client info
            CERR(send(sockfd, &msg, TCP_DATA_TOPICID + 1, 0) < 0);
        }
    }

    void accept_connection() {
        // Accept the new connection
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int new_sockfd =
            accept(main_tcp_sock, (sockaddr *)&client_addr, &client_len);
        CERR(new_sockfd < 0);

        // Add the new socket
        FD_SET(new_sockfd, &read_fds);
        max_fd = std::max(max_fd, (uint)new_sockfd);

        // TODO - If new client, save, else, make online
    }

   public:
    /**
     * @brief Initialise a new server on the spefied port
     * The main_port is the port that the server will listen for new connections
     * on
     * @param main_port The port
     */
    explicit Server(const uint main_port)
        : main_port(main_port), max_fd(0), max_topic_id(0) {
        // Initialise the main TCP socket
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        CERR(sock < 0);
        main_tcp_sock = sock;

        // Initialise the UDP socket
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        CERR(sock < 0);
        udp_sock = sock;

        // Clear the file descriptors sets
        clear_fds();

        // Set the socket options
        const int opt = 1;
        MUST(setsockopt(main_tcp_sock, SOL_SOCKET, TCP_NODELAY, &opt,
                        sizeof(opt) == 0),
             "Couldn't set master socket options\n");

        // Set the listen adress
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_port = htons(main_port);
        listen_addr.sin_addr.s_addr = INADDR_ANY;
    }

    ~Server() {
        // Close connections
        shutdown(main_tcp_sock, SHUT_RDWR);
        shutdown(udp_sock, SHUT_RDWR);
        close(main_tcp_sock);
        close(udp_sock);

        // TODO - close all client sockets
        close(max_fd);
    }

    /**
     * @brief Run the server
     */
    void run() {
        init_connections();
        do {
            tmp_fds = read_fds;
            CERR(select(max_fd + 1, &tmp_fds, NULL, NULL, NULL) < 0);
            for (uint i = 0; i <= max_fd; ++i) {
                if (FD_ISSET(i, &tmp_fds)) {
                    if (i == STDIN_FILENO) {
                        if (read_input()) {
                            // Close the program
                            return;
                        }
                    } else if (i == main_tcp_sock) {
                        accept_connection();
                    } else if (i == udp_sock) {
                        read_udp_message();
                    } else if (i != STDOUT_FILENO && i != STDERR_FILENO) {
                        read_tcp_message(i);
                    }
                }
            }
        }
        FOREVER;
    }
};
}  // namespace application