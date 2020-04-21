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

#include <iomanip>

#include "Utils.hpp"

/**
 * @brief This file contains definitions for different kinds of tcp/udp
 * messages.
 */

namespace application {
#pragma region UDP
struct udp_header {
    char topic[TOPIC_LENGTH];
    bint type;

    std::string print() {
        std::stringstream ss;
        ss << topic << " - ";
        switch (type) {
            case INT:
                ss << "INT";
                break;
            case SHORT_REAL:
                ss << "SHORT_REAL";
                break;
            case FLOAT:
                ss << "FLOAT";
                break;
            case STRING:
                ss << "STRING";
                break;
            default:
                break;
        }
        return ss.str();
    }
};

struct udp_int_msg {
    udp_header hdr;
    bint sign;
    uint val;

    long value() { return atol(print().c_str()); }

    std::string print() {
        std::stringstream ss;
        if (sign) {
            ss << "-";
        }
        ss << ntohl(val);
        return ss.str();
    }
};

struct udp_real_msg {
    udp_header hdr;
    short val;

    float value() {
        float res = (float)((val));
        res /= 100;
        return res;
    }

    std::string print() {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << value();
        return ss.str();
    }
};

struct udp_float_msg {
    udp_header hdr;
    bint sign;
    uint val;
    bint exp;

    float value() {
        float res, pow = power(10, exp);
        res = (float)(ntohl(val));
        res /= pow;
        if (sign) {
            return -res;
        }
        return res;
    }

    std::string print() {
        std::stringstream ss;
        ss << value();
        return ss.str();
    }
};

struct udp_string_msg {
    udp_header hdr;
    char payload[UDP_PAYLOAD_SIZE];

    std::string print() { return std::string(payload); }
};
#pragma endregion UDP

#pragma region TCP
struct tcp_message {
    bint type;
    char payload[UDP_MSG_SIZE];  // The biggest payloads are the udp messages
};

struct tcp_connect {
    char name[50];  // The id of the client
};

struct tcp_subscribe {
    char topic[50];
    bool sf;
};

struct tcp_unsubscribe {
    uint topic;  // By this point, the subscriber must know the topic id
};

struct tcp_topic_id {
    char topic[50];
    uint id;
};

#pragma endregion TCP

}  // namespace application