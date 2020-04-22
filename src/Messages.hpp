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
// Next few structs define different types of udp messages
/**
 * @brief A udp message that contains a INT
 */
struct udp_int {
    bint sign;
    uint val;

    long value() { return atol(print().c_str()); }

    std::string print() {
        std::stringstream ss;
        if (sign) {
            ss << "-";
        }
        ss << val;
        return ss.str();
    }
};

/**
 * @brief A udp message that contains a SHORT_REAL
 */
struct udp_real {
    sint val;

    float value() {
        float res = (float)(val);
        res /= 100;
        return res;
    }

    std::string print() {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << value();
        return ss.str();
    }
};

/**
 * @brief A udp message that contains a FLOAT
 */
struct udp_float {
    bint sign;
    uint val;
    bint exp;

    float value() {
        float res, pow = power(10, exp);
        res = (float)(val);
        res /= pow;
        if (sign) {
            return -res;
        }
        return res;
    }

    std::string print() {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(exp) << value();
        return ss.str();
    }
};

/**
 * @brief A udp message that contains a STRING
 */
struct udp_string {
    char payload[UDP_PAYLOAD_SIZE];

    std::string print() { return std::string(payload); }
};

/**
 * @brief Contains the header of a udp message
 * The topic and the type of the message
 */
struct udp_message {
    char topic[TOPIC_LENGTH];
    bint type;
    char payload[UDP_PAYLOAD_SIZE];

    std::string print() {
        std::stringstream ss;
        ss << topic << " - ";
        switch (type) {
            case INT: {
                ss << "INT - ";

                udp_int data;
                data.sign = payload[0];
                data.val = ntohl(*(uint*)(payload + 1));

                ss << data.print();
            } break;
            case SHORT_REAL: {
                ss << "SHORT_REAL - ";

                udp_real data;
                data.val = ntohs(*(sint*)(payload));
                
                ss << data.print();
            } break;
            case FLOAT: {
                ss << "FLOAT - ";

                udp_float data;
                data.sign = payload[0];
                data.val = ntohl(*(uint*)(payload + 1));
                data.exp = payload[5];

                ss << data.print();
            } break;
            case STRING: {
                ss << "STRING - ";

                udp_string data;
                bzero(&data, UDP_STRING_SIZE);
                memcpy(&data, payload, UDP_STRING_SIZE);
                ss << data.print();
            } break;
            default:
                break;
        }
        return ss.str();
    }
};

#pragma endregion UDP

#pragma region TCP

/**
 * @brief Defines a standard tcp message
 * Contains a type and the payload
 */
struct tcp_message {
    bint type;
    char payload[UDP_MSG_SIZE];  // The biggest payloads are the udp messages
};

// Next structs define different payload types

/**
 * @brief Data for a CONNECT
 * Contains the name of the client that wants to connect
 * client => server
 */
struct tcp_connect {
    char name[50];  // The id of the client
};

/**
 * @brief Data for a SUBSCRIBE
 * Contains the name of the topic and if the "store & forward" option should be
 * activated client => server
 */
struct tcp_subscribe {
    char topic[50];
    bool sf;
};

/**
 * @brief Data for a UNSUBSCRIBE
 * Contains the id of the topic the client wants to unsubscibe from
 * client => server
 */
struct tcp_unsubscribe {
    uint topic;  // By this point, the subscriber must know the topic id
};

/**
 * @brief Data for a TOPIC_ID
 * Contains the name of topic and its assigned id
 * server => client
 */
struct tcp_topic_id {
    char topic[50];
    uint id;
};

#pragma endregion TCP

}  // namespace application