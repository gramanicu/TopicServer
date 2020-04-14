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

#define lint __UINT_FAST64_TYPE__  // Long Int
#define uint __UINT_FAST32_TYPE__  // Unsigned Int
#define sint __UINT_FAST16_TYPE__  // Short Int
#define bint __UINT_FAST8_TYPE__   // Byte Int
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
