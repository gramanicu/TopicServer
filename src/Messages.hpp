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

#define UDP_INT_SIZE 56
#define UDP_REAL_SIZE 53
#define UDP_FLOAT_SIZE 57

namespace application {
struct udp_header {
    char topic[TOPIC_LENGTH];
    bint type;

    std::string print() {
        std::stringstream ss;
        ss << topic << " ";
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

    std::string print() {
        std::stringstream ss;
        float rez = (float)((val));
        rez /= 100;
        ss << std::fixed << std::setprecision(2) << rez;
        return ss.str();
    }
};

struct udp_float_msg {
    udp_header hdr;
    bint sign;
    uint val;
    bint exp;

    std::string print() {
        std::stringstream ss;
        float rez, pow = power(10, exp);
        rez = (float)(ntohl(val));
        rez /= pow;
        if (sign) {
            ss << "-";
        }
        ss << rez;
        return ss.str();
    }
};

struct udp_string_msg {
    udp_header hdr;
    char payload[PAYLOAD_MAX_SIZE];
};

}  // namespace application