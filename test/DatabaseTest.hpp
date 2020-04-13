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
#include "Database.hpp"
#include "Test.hpp"

namespace testing {
class DatabaseTest : public Test {
   public:
    bool run_tests() {
        return test_newfolder() && test_newfile() && test_deletefile() &&
               test_deletefolder();
    }

   private:
    application::Database db;

    bool test_newfolder() {
        std::string name = "./tfolder/t1/t2/t3/t4/";
        db.createDirectory(name);

        struct stat buffer;
        stat(name.c_str(), &buffer);
        return ASSERT_TRUE(S_ISDIR(buffer.st_mode),
                           "Folder structure was not created!\n");
    }

    bool test_newfile() {
        std::string name = "./tfolder/t1/t2/t3/t4/t5/file.txt";
        db.createFile(name);

        struct stat buffer;
        stat(name.c_str(), &buffer);
        return ASSERT_TRUE(S_ISREG(buffer.st_mode),
                           "File/Folders were not created!\n");
    }

    bool test_deletefile() {
        std::string name = "./tfolder/t1/t2/t3/t4/t5/file.txt";
        db.deleteFile(name);

        struct stat buffer;
        stat(name.c_str(), &buffer);
        return ASSERT_FALSE(S_ISREG(buffer.st_mode), "File was not deleted!\n");
    }

    bool test_deletefolder() {
        std::string name = "./tfolder/";
        // db.deleteDirectory(name);
        db.deleteDirectory("./tfolder/t1/t2/t3/t4/t5/");
        db.deleteDirectory("./tfolder/t1/t2/t3/t4/");
        db.deleteDirectory("./tfolder/t1/t2/t3/");
        db.deleteDirectory("./tfolder/t1/t2/");
        db.deleteDirectory("./tfolder/t1/");
        db.deleteDirectory("./tfolder/");

        struct stat buffer;
        stat(name.c_str(), &buffer);
        return ASSERT_FALSE(S_ISDIR(buffer.st_mode),
                            "Folder structure was not deleted!\n");
    }
};
}  // namespace testing