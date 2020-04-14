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

#include <map>
#include "Utils.hpp"

enum user_status { U_OFFLINE, U_ONLINE };

namespace application {
class User {
   private:
    std::string id;
    uint socket;
    bint status;

    /**
     * @brief This structure stores all the topics this user is subscribed to
     * The key of the map is the id of the topic
     * The first element of the value-pair is the "id" of the last message that
     * was sent to the user on that topic
     * The second element stores whether to client should receive all unsent
     * "messages" while it was disconnected
     */
    std::map<uint, std::pair<uint, bint>> topics;

   public:
    // Constructors
    User() : id(""), socket(0), status(U_OFFLINE) {}
    explicit User(const std::string id, const uint socket,
                  const user_status status = U_OFFLINE)
        : id(id), socket(socket), status(status){};

    // Copy-Constructor
    User(const User& other)
        : id(other.id),
          socket(other.socket),
          status(other.status),
          topics(other.topics) {}

    /**
     * @brief Set the status of the user
     * This will save the
     * @param isOnline
     */
    void set_status(const user_status isOnline) { status = isOnline; }

    /**
     * @brief Subscribe the user to a topic
     * @param topic The id of the topic to subscribe to
     * @param store If messages sent while the user is offline should be send
     * later, when he returns
     * @param last_msg The id of the last message sent to the user, on this
     * topic
     */
    void subscribe(const uint topic, const bool store, const uint last_msg) {
        std::pair<uint, bint> p;
        p = std::make_pair(last_msg, store);

        // This operation must not change existing values
        if (!is_subscribed(topic)) {
            // Add the new subscription
            topics[topic] = p;
        }
    }

    /**
     * @brief Unsubscribe the user from a topic
     * @param topic The topic to unsubscribe from
     */
    void unsubcribe(const uint topic) { topics.erase(topic); }

    /**
     * @brief Get the id of the user
     * @return uint The id of the user
     */
    std::string get_id() const { return id; }

    /**
     * @brief Get the socket on which this user is connected to the server
     * The socket of the server
     * @return uint The socket fd
     */
    uint get_socket() const { return socket; }

    /**
     * @brief Get the status of the user
     * @return user_status If he is online or offline
     */
    user_status get_status() const {
        if (status == 0) {
            return U_OFFLINE;
        } else {
            return U_ONLINE;
        }
    }

    /**
     * @brief Checks if a user is subscribed to a topic
     * @param topic The id of the topic
     * @return true The user is subscribed
     * @return false The user is not subscribed
     */
    bool is_subscribed(const uint topic) {
        auto it = topics.find(topic);
        return it != topics.end();
    }

    /**
     * @brief Checks if the user is online
     * @return true The user is online
     * @return false The user is not online
     */
    bool is_online() const {
        if (status == 0) {
            return false;
        } else {
            return true;
        }
    }

    /**
     * @brief Check if the user will receive unsent messages on the topic
     * @param topic The topic
     * @return true The user will receive unsent messages
     * @return false Unsent messages will be "forgot" for this user
     */
    bool get_store(const uint topic) const {
        if (topics.at(topic).second == 0) {
            return false;
        } else {
            return true;
        }
    }

    /**
     * @brief Get the id of the last message the user received on the topic
     * @param topic The topic
     * @return uint The id
     */
    uint get_last_id(const uint topic) const { return topics.at(topic).first; }

    /**
     * @brief Increment the id of the last message sent on the specified topic
     * @param topic The topic
     */
    void sent_message(const uint topic) { topics[topic].first++; }
};
}  // namespace application