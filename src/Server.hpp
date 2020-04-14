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
#include <netinet/ip.h>
#include <sys/select.h>
#include <iostream>
#include <string>
#include <vector>
#include "Database.hpp"
#include "Message.hpp"
#include "User.hpp"

namespace application {
class Server {
   private:
    uint main_port, main_tcp_sock, udp_sock, max_fd;
    fd_set read_fds, tmp_fds;
    sockaddr_in listen_addr;

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
        CERR(bind(main_tcp_sock, (sockaddr*)&listen_addr, sizeof(sockaddr)) <
             0);
        CERR(listen(main_tcp_sock, MAX_CLIENTS) < 0);
        CERR(bind(udp_sock, (sockaddr*)&listen_addr, sizeof(sockaddr)) < 0);

        // Set the file descriptors for the sockets
        FD_SET(main_tcp_sock, &read_fds);
        FD_SET(udp_sock, &read_fds);
        max_fd = std::max(main_tcp_sock, udp_sock);

        // Set the file descriptor for STDIN
        FD_SET(STDIN_FILENO, &read_fds);
    }

    void read_input() {
        std::string command;
        std::cin >> command;

        if (command == "exit") {
            // Close all clients
            exit(0);
        }
    }

    void read_udp_message() {}

    void read_tcp_message() {}

    void accept_connection() {}

   public:
    /**
     * @brief Initialise a new server on the spefied port
     * The main_port is the port that the server will listen for new connections
     * on
     * @param main_port The port
     */
    Server(const uint main_port) : main_port(main_port) {
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

        // Set the listen adress
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_port = htons(main_port);
        listen_addr.sin_addr.s_addr = INADDR_ANY;
    }

    ~Server() {
        // Close connections
        close(main_tcp_sock);
        close(udp_sock);
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
                        read_input();
                    } else if (i == main_tcp_sock) {
                        // New TCP connection
                    } else if (i == udp_sock) {
                        // New UDP message
                    } else if (i != STDOUT_FILENO && i != STDERR_FILENO) {
                        // New TCP message
                    }
                }
            }
        }
        FOREVER;
    }
};
}  // namespace application