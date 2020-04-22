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
#include "Topic.hpp"
#include "User.hpp"
#include "Utils.hpp"

namespace application {
/**
 * @brief This class manages the Database of the application
 * Users (CLIENT_ID's) and their data, topic data, and some other data used by
 * the application
 */
class Database {
   private:
    std::map<std::string, User> userList;
    std::map<uint, Topic> topics;
    uint max_topic_id;

    /**
     * @brief When a new connections is established, the server must wait the
     * client to send its id. Untill it happens, this data structure stores the
     * client's ip adress and port (in sockaddr_in).
     */
    std::map<uint, sockaddr_in> reservedAdresses;

   public:
    /**
     * @brief Default constructor
     */
    Database()
        : userList(std::map<std::string, User>()),
          topics(std::map<uint, Topic>()),
          max_topic_id(0),
          reservedAdresses(std::map<uint, sockaddr_in>()) {}

    /**
     * @brief Add a new user to the database
     * @param user The new user
     */
    void add_user(const User& user) {
        userList.insert(std::make_pair(user.get_id(), user));
    }

    /**
     * @brief Returns the user with the specified id (!check if user_exists
     * before!)
     * @param id The id
     * @return User& The user
     */
    User& get_user(const std::string& id) {
        auto i = userList.find(id);
        return i->second;
    }

    /**
     * @brief Get a vector with all users
     * @return std::vector<User&> All users
     */
    std::vector<User> get_users() {
        std::vector<User> v;
        for (auto i : userList) {
            v.push_back(i.second);
        }
        return v;
    }

    /**
     * @brief Get a vector with all online users
     *
     * @return std::vector<User> The online users
     */
    std::vector<User> get_online_users() {
        std::vector<User> v;
        for (auto i : userList) {
            if (i.second.is_online()) {
                v.push_back(i.second);
            }
        }
        return v;
    }

    /**
     * @brief Get a vector with all users subscribed to the speicified topic
     * @param topic The topic
     * @return std::vector<User> The subscribed users
     */
    std::vector<User> get_subscribed_users(const Topic& topic) {
        std::vector<User> v;
        for (auto i : userList) {
            if (i.second.is_subscribed(topic.get_id())) {
                v.push_back(i.second);
            }
        }
        return v;
    }

    /**
     * @brief Checks if a user with the specified id already exists
     * @param id The id of the user
     * @return true The user exist
     * @return false The user doesn't exist
     */
    bool user_exists(const std::string& id) const {
        auto i = userList.find(id);
        if (i != userList.end()) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief Disconnects the user with the specified sockfd
     * Disconnect - he will be offline, port = 0 and ip = ""
     * @param sockfd The socket of the user
     */
    void user_disconnect(const uint sockfd) {
        for (auto& i : userList) {
            if (i.second.get_socket() == sockfd) {
                i.second.disconnect();
            }
        }
    }

    /**
     * @brief Reserve the data for the specified socket
     * This is used to set the user port and ip when the server receives his id
     * @param sockfd The socket on which the client is connected
     * @param addr The data to reserve
     */
    void reserve_adress(const uint sockfd, sockaddr_in addr) {
        reservedAdresses.insert(std::make_pair(sockfd, addr));
    }

    /**
     * @brief Return the data reserved for the specified socket
     * @param sockfd The socket
     * @return sockaddr_in Adress data
     * Will return a sockaddr_in with sin_port set to -1 if the adress doesn't
     * exist
     */
    sockaddr_in get_reserved_adress(uint sockfd) {
        auto i = reservedAdresses.find(sockfd);
        if (i != reservedAdresses.end()) {
            sockaddr_in other = i->second;
            reservedAdresses.erase(i);
            return other;
        } else {
            sockaddr_in empty;
            empty.sin_port = -1;
            return empty;
        }
    }

    /**
     * @brief Return the name of a topic
     * Will return " " if the id was not sent by the server
     * @param id The id of the topic
     * @return std::string The name of the topic
     */
    std::string get_topic_name(uint id) {
        auto it = topics.find(id);

        if (it == topics.end()) {
            return " ";
        } else {
            return it->second.get_name();
        }
    }

    /**
     * @brief Return the id of a topic
     * Will return -1 if the topic id was not sent by the server
     * @param name The name of the topic
     * @return int The id of the topic
     */
    int get_topic_id(const std::string& name) {
        auto it = std::find_if(
            topics.begin(), topics.end(),
            [&name](auto&& pair) { return pair.second.get_name() == name; });

        if (it == topics.end()) {
            return -1;
        }
        return it->first;
    }

    /**
     * @brief Add a new message to the topic
     * @param id The id of the topic
     * @param message The message
     */
    void topic_new_message(uint id, std::string message) {
        // TODO - create topic if it doesn't exist
        auto it = topics.find(id);
        if (it != topics.end()) {
            it->second.add_message(message);
        }
    }

    /**
     * @brief Save all the topics messages from memory to the files
     */
    void save_topics() {
        for (auto& i : topics) {
            i.second.save();
        }
    }

    /**
     * @brief Add a new topic to the list
     * @param name The name of the topic
     * The id is automatically assigned
     */
    void add_topic(const std::string& name) {
        // If the topic doesn't exist already
        if (get_topic_id(name) == -1) {
            topics.insert(
                std::make_pair(max_topic_id, Topic(max_topic_id, name)));
            max_topic_id++;
        }
    }
};
}  // namespace application