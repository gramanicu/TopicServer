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

#include "Filesystem.hpp"
#include "Utils.hpp"

namespace database {
class Client {
   private:
    std::string id;
    bool isOnline;
    std::unordered_set<uint> topics;

   public:
    /**
     * @brief A new client with a specific id
     * He will be online by default, with no topics subscribed
     * @param _id The id
     */
    explicit Client(const std::string& _id)
        : id(_id), isOnline(true), topics(std::unordered_set<uint>()) {}

    /**
     * @brief Copy-constructor
     */
    Client(const Client& other)
        : id(other.id), isOnline(other.isOnline), topics(other.topics) {}

    /**
     * @brief Set the client's status
     * @param _isOnline If he is online or not
     */
    void set_status(const bool _isOnline) { isOnline = _isOnline; }

    /**
     * @brief Return the status of the client
     * @return true He is online
     * @return false He is offline
     */
    bool get_status() const { return isOnline; }

    /**
     * @brief Get the id of the client
     * @return std::string The id of the client
     */
    std::string get_id() const { return id; }

    /**
     * @brief Checks if the client is subscribed to a topic
     * @param topic The id of the topic
     * @return true The client is subscribed to the topic
     * @return false The client is not subscribed to the topic
     */
    bool is_subscribed(const uint topic) {
        auto it = topics.find(topic);
        return it != topics.end();
    }

    /**
     * @brief Subscribe a client to a topic
     * @param topic The id of the topic
     */
    void subscribe(const uint topic) { topics.insert(topic); }

    /**
     * @brief Unsubscribe a client from a topic
     * @param topic The id of the topic
     */
    void unsubscribe(const uint topic) { topics.erase(topic); }
};
}  // namespace database

namespace application {
using namespace database;

/**
 * @brief This class manages the Database of the application
 * Users (CLIENT_ID's) and their data, topic data, and some other data used by
 * the application
 */
class Database {
   private:
    std::vector<Client> clientList;

   public:
};
}  // namespace application