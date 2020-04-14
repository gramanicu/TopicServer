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
#include "Test.hpp"
#include "User.hpp"

namespace testing {
class UserTest : public Test {
   public:
    bool run_tests() {
        return test_offline() && test_online() && test_id() &&
               test_subscribe() && test_store() && test_messages() &&
               test_unsubscribe();
    }

   private:
    application::User user;
    uint topic = 12123127;
    uint lastmessage = 128;

    bool test_offline() {
        user = application::User(10, U_OFFLINE);
        return ASSERT_FALSE(user.is_online(), "The user should be offline\n");
    }

    bool test_online() {
        user.set_status(U_ONLINE);
        return ASSERT_TRUE(user.is_online(), "The user should be online\n");
    }

    bool test_id() {
        return ASSERT_EQUALS(user.get_id(), 10, "The user id is not correct\n");
    }

    bool test_subscribe() {
        user.subscribe(topic, false, lastmessage);
        return ASSERT_TRUE(user.is_subscribed(topic),
                           "The user didn't subscribe\n");
    }

    bool test_store() {
        return ASSERT_FALSE(user.get_store(topic),
                            "The user shouldn't receive unsent messages\n");
    }

    bool test_messages() {
        user.sent_message(topic);
        user.sent_message(topic);
        user.sent_message(topic);
        user.sent_message(topic);

        return ASSERT_EQUALS(user.get_last_id(topic), lastmessage + 4,
                             "The number of messages should have changed\n");
    }

    bool test_unsubscribe() {
        user.unsubcribe(topic);
        return ASSERT_FALSE(
            user.is_subscribed(topic),
            "The user should no longer be subscribed to that topic\n");
    }
};
}  // namespace testing