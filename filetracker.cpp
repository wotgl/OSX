#include "filetracker.hpp"

FileTracker::FileTracker(const char* directory) {
    this->directory = directory;

    struct dirent *ent;

    if ((dir = opendir(directory)) != NULL) {
        for (ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
            fileNames.push_back(ent->d_name);

        }
        closedir(dir);
    } else {
        std::cout << "Can not open directory";
        return;
    }

    for (int i = 0; i < fileNames.size(); ++i) {
        filesHash.insert(std::pair<std::string, size_t>(fileNames.at(i), getFileHash(fileNames.at(i))));
    }
}

FileTracker::~FileTracker() {

}

std::string FileTracker::readFile(std::string filename) {
    std::ifstream f(directory + filename);

    if (!f) {
        return "";
    }

    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
}

size_t FileTracker::getFileHash(std::string filename) {
    std::hash<std::string> h;
    return h(readFile(filename));
}

void FileTracker::trackFile(std::string filename) {
    for (;;) {
        size_t hash = getFileHash(filename);
        if (hash != filesHash.at(filename)) {
            std::lock_guard<std::mutex> lock(locker);
            std::cout << "FILE " << filename << " HAS BEEN EDITED!!!\n";
            filesHash.at(filename) = hash;
            locker.unlock();
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void FileTracker::checkDifference() {
    std::vector<std::thread> threads;
    for (int i = 0; i < fileNames.size(); ++i) {
        std::string filename = fileNames.at(i);
        threads.push_back(std::thread(&FileTracker::trackFile, this, filename));
    }
    for (int i = 0; i < threads.size(); ++i) {
        threads.at(i).join();
    }
}

void FileTracker::printFileNames() {
    for (auto iter = fileNames.begin(); iter != fileNames.end(); ++iter) {
        std::cout << *iter << "\n";
    }
}

void FileTracker::printFilesHash() {
    for (auto iter = filesHash.begin(); iter != filesHash.end(); ++iter) {
        std::cout << iter->first << " " << iter->second << "\n";
    }
}
