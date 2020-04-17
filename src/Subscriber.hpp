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

    /**
     * @brief Clear the file descriptors
     */
    void clear_fds() {
        FD_ZERO(&read_fds);
        FD_ZERO(&tmp_fds);
    }

    /**
     * @brief Connect to server and send client info
     */
    void init_connection() {
        // Connect to the server
        CERR(connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)));

        // Set the file descriptors for the sockets
        FD_SET(sockfd, &read_fds);

        // Set the file descriptor for STDIN
        FD_SET(STDIN_FILENO, &read_fds);

        // Send client info and wait for response
        char buffer[TCP_MSG_SIZE];
        bzero(buffer, TCP_MSG_SIZE);
        tcp_message msg;
        msg.type = tcp_msg_type::CONNECT;
        strcpy(msg.payload, client_id.c_str());

        memcpy(buffer, &msg, TCP_MSG_SIZE);

        // Send the client info
        CERR(send(sockfd, buffer, strlen(buffer), 0) < 0);
    }

    void read_tcp_message() {}

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
            std::string topic;
            bool sf;
            // If the second argument is not a number, the value will be false
            // If the second argument is greater than 1, the value will be true
            std::cin >> topic >> sf;

            // Subscribe
        } else if (command == "unsubscribe") {
            // Unsubscribe
            std::string topic;
            std::cin >> topic;
        }
        return false;
    }

   public:
    Subscriber(const std::string id, const char* ip, const uint port)
        : server_port(port), client_id(id) {
        // Initialise the TCP socket
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        MUST(sock >= 0, "Failed to initialise socket\n");
        sockfd = sock;

        // Clear the file descriptors sets
        clear_fds();

        // Set the server adress
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port);
        MUST(inet_aton(ip, &server_addr.sin_addr) != 0, "Invalid IP adress\n");
    }

    ~Subscriber() {
        // Close connection
        close(sockfd);
    }

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
                            return;
                        }
                    } else if (i != sockfd) {
                        read_tcp_message();
                    }
                }
            }
        }
        FOREVER;
    }
};
}  // namespace application