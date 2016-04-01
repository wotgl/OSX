#include "filetracker.hpp"

FileTracker::FileTracker(const char *directory) {
    this->directory = directory;
    struct dirent *ent;

    if ((dir = opendir(directory)) != NULL) {
        for (ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
            fileNames.push_back(ent->d_name);
        }
    } else {
        std::cout << "Can not open directory";
        return;
    }
    closedir(dir);
    for (int i = 0; i < fileNames.size(); ++i) {
        filesHash.insert(std::pair<std::string, size_t>(
            fileNames.at(i), getFileHash(fileNames.at(i))));
    }
}

FileTracker::~FileTracker() {}

std::string FileTracker::readFile(std::string filename) {
    std::ifstream f(directory + filename);

    if (!f) {
        locker.lock();
        std::cout << "File " << filename << " has been removed!\n";
        locker.unlock();
        fileNames.erase(
            std::remove(fileNames.begin(), fileNames.end(), filename),
            fileNames.end());
        filesHash.erase(filename);
        return "";
    }

    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
}

size_t FileTracker::getFileHash(std::string filename) {
    std::hash<std::string> h;
    std::string file = readFile(filename);
    if (file == "") {
        return 0;  // hash returns 0 if the file is empty
    }
    return h(readFile(filename));
}

void FileTracker::trackFilesNotMulti() {
    for (;;) {
        for (auto iter = fileNames.begin(); iter != fileNames.end(); ++iter) {
            size_t hash = getFileHash(*iter);
            if (hash != filesHash.at(*iter)) {
                locker.lock();
                std::cout << "File " << *iter << " has been edited!\n";
                locker.unlock();
                filesHash.at(*iter) = hash;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void FileTracker::checkFileAdd() {
    for (;;) {
        dir = opendir(directory);
        struct dirent *ent;
        for (ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
            if (!(std::find(fileNames.begin(), fileNames.end(), ent->d_name) !=
                  fileNames.end())) {
                locker.lock();
                std::cout << "File " << ent->d_name << " has been created!\n";
                locker.unlock();
                fileNames.push_back(ent->d_name);

                filesHash.insert(std::pair<std::string, size_t>(
                    ent->d_name, getFileHash(ent->d_name)));
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        closedir(dir);
    }
}

void FileTracker::checkDifferenceNotMulti() {
    std::thread addChecker(&FileTracker::checkFileAdd, this);
    std::thread fileChecker(&FileTracker::trackFilesNotMulti, this);

    addChecker.join();
    fileChecker.join();
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
