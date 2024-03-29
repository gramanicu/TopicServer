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

#include <arpa/inet.h>
#include <memory.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define lint uint64_t  // Long Int
#define uint uint32_t  // Unsigned Int
#define sint uint16_t  // Short Int
#define bint uint8_t   // Byte Int
#define uchar unsigned char

#define FOREVER while (1)

/**
 * @brief Check if the condition is met. If it doesn't, print message and exit
 */
#define MUST(condition, message) \
    if (!(condition)) {          \
        std::cerr << message;    \
        exit(-1);                \
    }

/**
 * @brief Check if the error happens. If it does, print it
 */
#define CERR(condition)                                   \
    if (condition && ENABLE_LOGS) {                       \
        std::cerr << __FILE__ << ", " << __LINE__ << ": " \
                  << std::strerror(errno) << "\n";        \
    }

// Server settings
#define ENABLE_LOGS false
#define DATABASE_FOLDER "./data/"

// Server constants
#define MAX_CLIENTS UINT32_MAX
#define MAX_STDIN_COMMAND 100

// Message constants
#define TOPIC_LENGTH 50
#define UDP_MSG_SIZE 1551
#define UDP_PAYLOAD_SIZE 1500
#define TCP_MSG_SIZE sizeof(tcp_message)
#define TCP_DATA_DATA 1596
#define TCP_DATA_SUBSCRIBE sizeof(tcp_subscribe)
#define TCP_DATA_UNSUBSCRIBE sizeof(tcp_unsubscribe)
#define TCP_DATA_CONFIRM_U sizeof(tcp_confirm_u)
#define TCP_DATA_TOPICID sizeof(tcp_topic_id)
#define TCP_DATA_CONNECT 50
#define UDP_INT_SIZE sizeof(udp_int)
#define UDP_REAL_SIZE sizeof(udp_real)
#define UDP_FLOAT_SIZE sizeof(udp_float)
#define UDP_STRING_SIZE sizeof(udp_string)

/**
 * @brief The different types of UDP messages, as defined in the problem
 * statement
 */
enum udp_msg_type { INT, SHORT_REAL, FLOAT, STRING };

/**
 * @brief The different types of TCP messages
 * DATA - server->client - contains topic data
 * SUBSCRIBE - client->server - client subscription request
 * UNSUBSCRIBE - client->server - client unsubscribe request
 * TOPIC_ID - server->client - contains the id of a specific topic
 * CONNECT - client->server - client connect request and his info (id, etc.)
 * CONFIRM_U - server->client - servers confirms that the client was
 * unsubscribed
 * CONNECT_DUP - server->client - notifies that the client is already connected
 */
enum tcp_msg_type {
    DATA,
    SUBSCRIBE,
    UNSUBSCRIBE,
    TOPIC_ID,
    CONNECT,
    CONFIRM_U,
    CONNECT_DUP
};

// Compute power y of x in O(Log y)
double power(int x, uint y) {
    double res = 1.0;

    while (y > 0) {
        if (y & 1) {
            res = res * x;
        }
        y = y >> 1;
        x = x * x;
    }
    return res;
}

/**
 * @brief This function is similar to strncpy
 * Because strcpy is vulnerable to buffer overflows and strncpy doesn't
 * necessarily end strings with null terminator, this is solved using this
 * function. Works like strlcpy
 */
void safe_cpy(char *dst, const char *src, size_t size) {
    *((char *)mempcpy(dst, src, size)) = '\0';
}

/**
 * @brief Print messages to STDOUT
 * Will only print if ENABLE_LOGS is true
 * @param msg The message to be printed
 */
void console_log(const std::string &msg) {
    if (ENABLE_LOGS) {
        std::cout << msg;
    }
}

/**
 * @brief Sleep for the specified time in nanoseconds
 * @param nanoseconds
 */
void nsleep(long nanoseconds) {
    timespec slptm;
    slptm.tv_sec = 0;
    slptm.tv_nsec = nanoseconds;
    nanosleep(&slptm, NULL);
}