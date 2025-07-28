#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

class List_Video_Files {
public:
    List_Video_Files() = default;
    ~List_Video_Files() = default;

    std::vector<std::string> list_video_files(const std::string& dirPath) {
        fileNames.clear();
        scan_directory(dirPath);
        return fileNames;
    }

private:
    std::vector<std::string> fileNames;

    void scan_directory(const std::string& dirPath) {
        DIR* dir = opendir(dirPath.c_str());
        if (!dir) {
            std::cerr << "Cannot open directory: " << dirPath << "\n";
            return;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            process_entry(dirPath, entry);
        }

        closedir(dir);
    }

    void process_entry(const std::string& dirPath, struct dirent* entry) {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            return;

        std::string fullPath = dirPath + "/" + name;
        struct stat st;
        if (stat(fullPath.c_str(), &st) != 0)
            return;

        if (S_ISDIR(st.st_mode)) {
            std::cout << "Found directory: " << fullPath << std::endl;
            scan_directory(fullPath); // recursive call
        }
        else if (S_ISREG(st.st_mode)) {
            std::cout << "Found file: " << name << std::endl;
            fileNames.push_back(name);
        }
    }
};
