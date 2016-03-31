#pragma once

#include <dirent.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

class FileTracker {
   public:
    FileTracker(const char* directory);
    ~FileTracker();

    void printFileNames();
    void printFilesHash();
    void checkDifference();  // function that checks difference.

    // checkDifferenceNotMulti checks difference of directory.
    //
    // It creates two threads: first tracks edition of files, second tracks
    // creation of files. Deletion of files is tracked in first thread.
    void checkDifferenceNotMulti();
    void checkFileAdd();

   private:
    DIR* dir;                            // directory we scan
    const char* directory;               // the directory we are in.
    std::vector<std::string> fileNames;  // we get file names, then scan them.
    std::map<std::string, size_t> filesHash;
    size_t getFileHash(std::string filename);
    std::string readFile(std::string filename);  // we read file and get string

    void trackFilesNotMulti();

    std::mutex locker;
};
