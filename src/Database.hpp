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

#include <libgen.h>  // dirname, basename
#include <memory.h>
#include <sys/stat.h>  // mkdir
#include <sys/types.h>
#include <unistd.h>  // unlink, rmdir
#include <cstring>
#include <fstream>
#include <stack>

namespace application {

/**
 * @brief This class manages the database of the application
 * Users (CLIENT_ID's) and their data, topic data, and some other data used by
 * the application
 */
class Database {
   private:
    // Creates a folder, with the specified path
    // The last folder in the path (the one we want to create) is the only one
    // that DOESN'T exist
    void _createFolder(const std::string& path) {
        // Try to create the folder, with RWX permission for owner and
        // group, read for the others

        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH) != 0) {
            std::cout << "Folder creation failed (" << __FILE__ << ":"
                      << __LINE__ << "): " << std::strerror(errno) << "\n";
            exit(1);
        }
    }

    // Creates a folder with the specified path. Creates all folders in path
    // that don't exist.
    void _createFolders(const std::string& _path) {
        struct stat st;
        stat(_path.c_str(), &st);
        std::stack<std::string> paths;

        // Create a copy of the full path (must be done, as dirname
        // changes the path)
        char* path = (char*)malloc((_path.size() + 1) * sizeof(char));
        memcpy(path, _path.c_str(), (_path.size() + 1) * sizeof(char));

        // Check if directory tree doesn't exist
        while (!S_ISDIR(st.st_mode)) {
            paths.push(std::string(path));
            dirname(path);
            stat(path, &st);
        }
        // We found a path that does exist

        // Build all paths
        while (!paths.empty()) {
            _createFolder(paths.top());
            paths.pop();
        }

        free(path);
    }

    // Removes a file at the specified path
    void _removeFile(const std::string& _path) {
        // TODO - if file does not exist, ignore
        if (unlink(_path.c_str()) != 0) {
            std::cout << "File removal failed (" << __FILE__ << ":" << __LINE__
                      << "): " << std::strerror(errno) << "\n";
            exit(1);
        }
    }

    // Remove a folder from the specified path
    void _removeFolder(const std::string& _path) {
        // TODO - if file does not exist, ignore
        if (rmdir(_path.c_str()) != 0) {
            std::cout << _path << "\n";
            std::cout << "Directory removal failed (" << __FILE__ << ":"
                      << __LINE__ << "): " << std::strerror(errno) << "\n";
            exit(1);
        }
    }

   public:
    /**
     * @brief Creates a new file with the specified path
     * Can use both relative and absolute paths. If the folders in the
     * path do not exist, it will create them
     * @param _path The path of the file
     */
    void createFile(const std::string& _path) {
        char* path = (char*)malloc((_path.size() + 1) * sizeof(char));
        memcpy(path, _path.c_str(), (_path.size() + 1) * sizeof(char));

        if (_path.at(_path.size() - 1) != '/') {
            // If we may need to create directories, but definetely a file
            _createFolders(dirname(path));
            std::ofstream file{_path.c_str()};
        }
        free(path);
    }

    /**
     * @brief Creates a new directory/folder with the specified path
     * Can use both relative and absolute paths. If the folders in the
     * path do not exist, it will create them
     * @param _path The path of the folder
     */
    void createDirectory(const std::string& _path) {
        char* path = (char*)malloc((_path.size() + 1) * sizeof(char));
        memcpy(path, _path.c_str(), (_path.size() + 1) * sizeof(char));

        if (_path.at(_path.size() - 1) == '/') {
            // If we need to create only directories
            _createFolders(path);
        }
        free(path);
    }

    /**
     * @brief Delete a file with the specified path
     * If the path is not a file, nothing will be changed
     * @param _path The path to the file
     */
    void deleteFile(const std::string& _path) {
        char* path = (char*)malloc((_path.size() + 1) * sizeof(char));
        memcpy(path, _path.c_str(), (_path.size() + 1) * sizeof(char));

        if (_path.at(_path.size() - 1) != '/') {
            // This must not be a directory
            _removeFile(path);
        }
        free(path);
    }

    /**
     * @brief Delete a directory with the specified path (ending with a /)
     * If the path ends with a file, nothing will change
     * Attention - this will delete a directory and all it's contents
     * @param _path
     */
    void deleteDirectory(const std::string& _path) {
        // TODO - search recursively

        char* path = (char*)malloc((_path.size() + 1) * sizeof(char));
        memcpy(path, _path.c_str(), (_path.size() + 1) * sizeof(char));

        if (_path.at(_path.size() - 1) == '/') {
            // If the path is a directory
            _removeFolder(path);
        }
        free(path);
    }
};
}  // namespace application