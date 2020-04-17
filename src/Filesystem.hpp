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

#include <libgen.h>    // dirname, basename
#include <sys/stat.h>  // mkdir
#include <unistd.h>    // unlink, rmdir

#include <experimental/filesystem>  // remove_all

#include "Utils.hpp"

namespace application {

/**
 * @brief This class manages the filesystem of the application
 * It handles directory/file creation/deletion
 */
class Filesystem {
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
        if (unlink(_path.c_str()) != 0) {
            // If file does not exist, ignore the error
            if (errno != ENOENT) {
                std::cout << "File removal failed (" << __FILE__ << ":"
                          << __LINE__ << "): " << std::strerror(errno) << "\n";
                exit(1);
            }
        }
    }

    // Remove a folder (recursively) from the specified path
    void _removeFolders(const std::string& _path) {
        using namespace std::experimental::filesystem;
        const path p = _path;
        remove_all(p);
    }

/**
 * @brief Checks if files from a specified path can be changed.
 * To make sure that the program doesn't change any file that doesn't "own"
 * paths given to functions like "createFile" "deleteDirectory", etc . will
 * be checker. A path is valid only if it is included in the programs path.
 * @param _path
 * @return true The files at the specified path can be changed
 * @return false The files at specified path should not be modified
 */
#pragma GCC push_options  // Disable optimisations for this function
#pragma GCC optimize("-O0")
    bool _isValidPath(const std::string& _path, const bool must_exist) {
        // Check if file exists
        if (must_exist) {
            struct stat st;
            if (stat(_path.c_str(), &st) == -1) {
                if (errno == ENOENT) {
                    return false;
                }
            }
        }

        using namespace std::experimental::filesystem;
        std::string programPath = current_path();
        char* absolutePath = realpath(_path.c_str(), NULL);

        if (absolutePath == NULL) {
            if (errno == EACCES) {
                return false;
            } else if (errno != ENOENT) {
                std::cout << "Path checking failed (" << __FILE__ << ":"
                          << __LINE__ << "): " << std::strerror(errno) << "\n";
                exit(1);
            }
            return true;
        } else {
            // Check if the path is valid (meets the specified characteristics)
            // As the paths are absolute, we can simply verify if the checked
            // path starts with the programs path
            return strncmp(absolutePath, programPath.c_str(),
                           programPath.size()) == 0;
        }
    }
#pragma GCC pop_options

   public:
    /**
     * @brief Creates a new file with the specified path
     * Can use both relative and absolute paths. If the folders in the
     * path do not exist, it will create them
     * @param _path The path of the file
     */
    void createFile(const std::string& _path) {
        // If we don't have "rights" over files at that path
        if (!_isValidPath(_path, false)) {
            return;
        }

        char* path = (char*)malloc((_path.size() + 1) * sizeof(char));
        memcpy(path, _path.c_str(), (_path.size() + 1) * sizeof(char));

        if (_path.at(_path.size() - 1) != '/') {
            // If we may need to create directories, but definetely a file
            _createFolders(dirname(path));
            std::ofstream out{_path.c_str()};
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
        // If we don't have "rights" over files at that path
        if (!_isValidPath(_path, false)) {
            return;
        }

        char* path = (char*)malloc((_path.size() + 1) * sizeof(char));
        memcpy(path, _path.c_str(), (_path.size() + 1) * sizeof(char));
        _createFolders(path);
        free(path);
    }

    /**
     * @brief Delete a file with the specified path
     * If the path is not a file, nothing will be changed
     * @param _path The path to the file
     */
    void deleteFile(const std::string& _path) {
        // If we don't have "rights" over files at that path
        if (!_isValidPath(_path, true)) {
            return;
        }

        char* path = (char*)malloc((_path.size() + 1) * sizeof(char));
        memcpy(path, _path.c_str(), (_path.size() + 1) * sizeof(char));

        if (_path.at(_path.size() - 1) != '/') {
            // This must not be a directory
            _removeFile(path);
        }
        free(path);
    }

    /**
     * @brief Delete a directory with the specified path
     * If the path ends with a file, nothing will change
     * Attention - this will delete a directory and all it's contents
     * @param _path
     */
    void deleteDirectory(const std::string& _path) {
        // If we don't have "rights" over files at that path
        if (!_isValidPath(_path, true)) {
            return;
        }

        char* path = (char*)malloc((_path.size() + 1) * sizeof(char));
        memcpy(path, _path.c_str(), (_path.size() + 1) * sizeof(char));

        _removeFolders(path);
        free(path);
    }

    /**
     * @brief Checks if a path is inside the programs directory
     * @param _path The path to be checker
     * @return true The path is valid
     * @return false Files at this path cannot be changed
     */
    bool checkPath(const std::string& _path) {
        return _isValidPath(_path, true);
    }
};
}  // namespace application