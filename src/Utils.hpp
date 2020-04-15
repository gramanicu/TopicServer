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
    if (condition) {                                      \
        std::cerr << __FILE__ << ", " << __LINE__ << ": " \
                  << std::strerror(errno) << "\n";        \
    }

// Server constants
#define MAX_CLIENTS UINT32_MAX
#define MAX_STDIN_COMMAND 100

// Message constants
#define TOPIC_LENGTH 50
#define PAYLOAD_MAX_SIZE 1500
#define UDP_MSG_SIZE 1551
#define UDP_HDR_SIZE 51

enum msg_type { INT, SHORT_REAL, FLOAT, STRING };

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