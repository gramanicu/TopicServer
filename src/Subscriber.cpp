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

#include "Subscriber.hpp"

#include <iostream>
#include <sstream>

std::string require_params() {
    std::stringstream ss;
    ss << "Wrong parameters : ./subscriber ID IP PORT\n";
    return ss.str();
}

int main(int argc, char *argv[]) {
    // Check if the PORT parameter was specified
    MUST(argc == 4, require_params());

    // Checks if the port provided is an actual number
    uint port = atoi(argv[3]);
    MUST(port, require_params());

    // Check if the id is valid
    MUST(strlen(argv[1]) < 11, "Invalid ID (max 10 chars)\n");

    application::Subscriber subscriber(std::string(argv[1]), argv[2], port);
    subscriber.run();

    return 0;
}