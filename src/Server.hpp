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
    Database db;

    // The database that links topic names to their id's
    std::unordered_map<uint, std::string> topics;

    /**
     * @brief Clear the file descriptors
     */
    void clear_fds() {
        FD_ZERO(&read_fds);
        FD_ZERO(&tmp_fds);
    }

    /**
     * @brief Close a socket
     * @param sockfd The socket to be closed
     */
    void close_skt(int sockfd) {
        CERR(shutdown(sockfd, SHUT_RDWR) != 0);
        CERR(close(sockfd) != 0);
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
            return true;
        }
        return false;
    }

    /**
     * @brief This function parses and does different things based on UDP
     * messages it receives
     */
    void read_udp_message() {
        udp_message msg;
        char buffer[UDP_MSG_SIZE];
        bzero(&msg, UDP_MSG_SIZE);
        bzero(&buffer, UDP_MSG_SIZE);

        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        ssize_t msg_size = recvfrom(udp_sock, &buffer, sizeof(buffer), 0,
                                    (sockaddr *)&client_addr, &client_len);
        CERR(msg_size < 0);

        memcpy(&msg, buffer, msg_size);

        // This will build the string to be shown in the console log
        std::stringstream ss;
        ss << inet_ntoa(client_addr.sin_addr) << ":";
        ss << ntohs(client_addr.sin_port) << " - ";

        if (msg_size > 0) {
            ss << msg.print() << "\n";
            db.topic_new_message(db.get_topic_id(msg.topic), ss.str());
            console_log(ss.str());

            // TODO - send the message forward to the clients that require it
        }
    }

    /**
     * @brief This function parses and does different things based on TCP
     * messages it receives
     * @param sockfd The socket on which the message will be received
     */
    void read_tcp_message(uint sockfd) {
        tcp_message msg;
        bzero(&msg, TCP_MSG_SIZE);

        ssize_t msg_size = recv(sockfd, &msg, sizeof(msg), 0);
        CERR(msg_size < 0);

        if (msg_size == 0) {
            // Client disconnected
            close_skt(sockfd);
            FD_CLR(sockfd, &read_fds);
            db.user_disconnect(sockfd);
        } else {
            switch (msg.type) {
                case tcp_msg_type::CONNECT: {
                    tcp_connect data;
                    bzero(&data, TCP_DATA_CONNECT);
                    memcpy(&data, msg.payload, TCP_DATA_CONNECT);

                    sockaddr_in client_addr = db.get_reserved_adress(sockfd);
                    User user = User(
                        data.name, std::string(inet_ntoa(client_addr.sin_addr)),
                        sockfd, ntohs(client_addr.sin_port));

                    if (!db.user_exists(data.name)) {
                        // New user - add him to the database
                        db.add_user(user);

                        std::cout << "New client " << user.get_id()
                                  << " connected from " << user.get_ip() << ":"
                                  << user.get_port() << ".\n";
                    } else {
                        // Reconnected - just update the adress and port
                        std::cout << "Reconnected client " << user.get_id()
                                  << " from " << user.get_ip() << ":"
                                  << user.get_port() << ".\n";
                        if (db.user_exists(user.get_id())) {
                            db.get_user(user.get_id()) = user;
                        }

                        // TODO - sent queued messages
                    }
                } break;
                case tcp_msg_type::SUBSCRIBE: {
                    tcp_subscribe data;
                    bzero(&data, TCP_DATA_SUBSCRIBE);
                    memcpy(&data, msg.payload, TCP_DATA_SUBSCRIBE);

                    std::cout << "Subscribed " << data.topic << "  " <<  data.sf << "\n";

                    // Add the topic if it doesn't exist already
                    db.add_topic(data.topic);
                    // Send the id of the topic to the client
                    send_topic_id(sockfd, data.topic);
                } break;
                case tcp_msg_type::UNSUBSCRIBE: {
                    tcp_unsubscribe data;
                    bzero(&data, TCP_DATA_UNSUBSCRIBE);
                    memcpy(&data, msg.payload, TCP_DATA_UNSUBSCRIBE);

                    std::cout << "Unsubscribed " << data.topic << "\n";

                    // TODO - send ACK to client
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

        int id = db.get_topic_id(name);
        if (id != -1) {
            tcp_topic_id data;
            bzero(&data, TCP_DATA_TOPICID);

            safe_cpy(data.topic, name.c_str(), name.size());
            data.id = id;

            msg.type = tcp_msg_type::TOPIC_ID;
            memcpy(msg.payload, &data, TCP_DATA_TOPICID);

            // Send the client info
            CERR(send(sockfd, &msg, TCP_DATA_TOPICID + 1, 0) < 0);
        }
    }

    /**
     * @brief This function manages new TCP connections
     */
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

        // Reserve the user data
        db.reserve_adress(new_sockfd, client_addr);
    }

   public:
    /**
     * @brief Initialise a new server on the spefied port
     * The main_port is the port that the server will listen for new connections
     * on
     * @param main_port The port
     */
    explicit Server(const uint main_port)
        : main_port(main_port), max_fd(0), db(Database()) {
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
        close_skt(main_tcp_sock);

        // Close all client sockets
        for (User &usr : db.get_online_users()) {
            std::cout << usr.get_socket() << "\n";
            close_skt(usr.get_socket());
        }

        db.save_topics();
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