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
#include <iostream>
#include <string>

#define ASSERT_TRUE(statement, message) \
    testing::Test::run_test(statement, message);
#define ASSERT_FALSE(statement, message) \
    testing::Test::run_test(!statement, message);
#define ASSERT_EQUALS(first, second, message) \
    testing::Test::run_test(first == second, message);

namespace testing {
/**
 * @brief An abstract class used for tests
 */
class Test {
   public:
    virtual ~Test() {}
    virtual bool run_tests() = 0;

   protected:
    /**
     * @brief Run a test. If it returns false, print the fail_message
     * Will return the test result.
     * @param test_result The result of a test
     * @param fail_message The message to show if the test failed
     * @return true
     * @return false
     */
    bool run_test(const bool test_result, const std::string fail_message) {
        if (!test_result) {
            std::cerr << fail_message << "\n";
            return false;
        }
        return true;
    }
};
}  // namespace testing