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

#define MAX_TOPIC_LINES 500

namespace application {
class Topic {
   private:
    uint id;
    std::string name;
    long last_message_id;
    std::queue<std::string> messages;

    /**
     * @brief Get the id from a message
     * @param msg The message
     * @return uint Its id
     */
    uint get_message_id(const std::string& msg) {
        std::istringstream ss(msg);
        std::string id;
        ss >> id;
        return atoi(id.c_str());
    }

    /**
     * @brief Return the message with the specified id (from the memory or the
     * filesystem)
     * @param msg_id The id of the message
     * @return std::string The message
     */
    std::string get_message(const uint msg_id) {
        // If the id we search is smaller than the oldest message from the
        // queue, it means we can find it in the file
        std::string first_msg = messages.front();

        if (get_message_id(first_msg) > msg_id) {
            std::ifstream in(DATABASE_FOLDER + name);

            std::string data;
            while (std::getline(in, data)) {
                if (get_message_id(data) == msg_id) {
                    return data;
                }
            }
        } else {
            // The message is in the memory
            std::queue<std::string> other = messages;
            while (!other.empty()) {
                if (get_message_id(other.front()) == msg_id) {
                    return other.front();
                } else {
                    other.pop();
                }
            }
        }

        return "";
    }

   public:
    Topic()
        : id(0),
          name(""),
          last_message_id(-1),
          messages(std::queue<std::string>()) {
        Filesystem fs;
        fs.createFile(DATABASE_FOLDER);
    }

    /**
     * @brief Construct a new topic
     * Also, create the file that will store this topic's messages
     * @param id The id of the topic (set by the server)
     * @param name The name of the topic
     */
    Topic(const uint id, const std::string& name)
        : id(id),
          name(name),
          last_message_id(-1),
          messages(std::queue<std::string>()) {
        Filesystem fs;
        fs.createFile(DATABASE_FOLDER + name);
    }

    /**
     * @brief Copy constructor
     * @param other Another Topic object
     */
    Topic(const Topic& other)
        : id(other.id),
          name(other.name),
          last_message_id(other.last_message_id),
          messages(other.messages) {
        // It doesn't need to create any new file
    }

    uint get_id() const { return id; }

    std::string get_name() const { return name; }

    void add_message(const std::string& message) {
        // If there are too many messages in the stack, store excess messages in
        // file
        std::ofstream out(DATABASE_FOLDER + name,
                          std::ios_base::app | std::ios_base::out);
        if (messages.size() == MAX_TOPIC_LINES) {
            // Store a quarter of the messages
            for (int i = 0; i < MAX_TOPIC_LINES / 4; ++i) {
                out << messages.front() << "\n";
                messages.pop();
            }
            out.close();
        }

        out.close();
        last_message_id++;
        messages.push(std::to_string(last_message_id) + " " + message);
    }

    /**
     * @brief Store all data into files. Will remove it from memory
     */
    void save() {
        std::ofstream out(DATABASE_FOLDER + name,
                          std::ios_base::app | std::ios_base::out);
        // Store a quarter of the messages
        while (!messages.empty()) {
            out << messages.front() << "\n";
            messages.pop();
        }
        out.close();
    }

    /**
     * @brief Returns all the messages with id's in the specified range
     * @param start The smaller id
     * @param finish The greater id
     * @return std::vector<std::string> An array of messages
     */
    std::vector<std::string> get_messages(uint start, uint finish) {
        std::vector<std::string> v;

        // Make the range valid if it isn't
        if (start > finish) {
            std::swap(start, finish);
        }

        if (finish > last_message_id) {
            finish = last_message_id;
        }

        // Get all messages from the range
        for (uint i = start; i <= finish; ++i) {
            // Remove the message id from the messages
            std::string msg = get_message(i);
            int pos = msg.find_first_of(' ');
            if (msg != "") {
                v.push_back(msg.substr(pos + 1));
            }
        }

        return v;
    }

    /**
     * @brief Get the last message on the topic
     * @return std::string The message
     */
    std::string get_last_message() const { return messages.front(); }

    /**
     * @brief Get the id of the last message
     * @return uint The message id
     * Id is -1 when there are no messages on the topic
     */
    long get_last_id() const { return last_message_id; }
};
}  // namespace application