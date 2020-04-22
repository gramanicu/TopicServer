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
    lint last_message_id;
    std::queue<std::string> messages;

   public:
    /**
     * @brief Construct a new topic
     * Also, create the file that will store this topic's messages
     * @param id The id of the topic (set by the server)
     * @param name The name of the topic
     */
    Topic(const uint id, const std::string& name)
        : id(id),
          name(name),
          last_message_id(0),
          messages(std::queue<std::string>()) {
        Filesystem fs;
        fs.createFile("./data/" + name);
    }

    uint get_id() const { return id; }

    std::string get_name() const { return name; }

    void add_message(std::string message) {
        // If there are too many messages in the stack, store excess messages in
        // file
        std::ofstream out("./data/" + name,
                          std::ios_base::app | std::ios_base::out);
        if (messages.size() == MAX_TOPIC_LINES) {
            // Store a quarter of the messages
            for (int i = 0; i < MAX_TOPIC_LINES / 4; ++i) {
                out << messages.front() << "\n";
                messages.pop();
            }
            out.close();
        }

        messages.push(message);
    }

    /**
     * @brief Store all data into files. Will remove it from memory
     */
    void save() {
        std::ofstream out("./data/" + name,
                          std::ios_base::app | std::ios_base::out);
        // Store a quarter of the messages
        while (!messages.empty()) {
            out << messages.front() << "\n";
            messages.pop();
        }
        out.close();
    }

    std::string get_last_message() const { return messages.front(); }
};
}  // namespace application