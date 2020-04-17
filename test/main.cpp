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

#include <iostream>
#include <vector>

#include "FilesystemTest.hpp"
#include "UserTest.hpp"

/**
 * @brief Runs the tests
 * Will print how many tests passed.
 * @return int 0 if no tests failed, -1 if any test failed
 */
int main() {
    std::vector<testing::Test*> tests;

    // Add tests to be run
    tests.push_back(new testing::FilesystemTest());
    tests.push_back(new testing::UserTest());

    // Do not change code from here
    // If it has any tests to run
    if (tests.size() > 0) {
        unsigned int cTest = 0;
        for (auto& i : tests) {
            if (i->run_tests()) {
                cTest++;
            }
        }

        std::cout << "Passed " << cTest << "/" << tests.size() << " tests!\n";
        if (cTest == tests.size()) {
            return 0;
        } else {
            return -1;
        }
    } else {
        std::cout << "No test to be run\n";
        return 0;
    }
}