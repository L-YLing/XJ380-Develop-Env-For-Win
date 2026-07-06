#pragma once

#include <string>
#include <vector>

namespace utils {
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    bool createDirectory(const std::string& path);
    bool deleteFile(const std::string& path);
    bool deleteDirectory(const std::string& path);
    std::string getCurrentDirectory();
    std::string getTempDirectory();
    std::string combinePaths(const std::string& path1, const std::string& path2);
    std::string getFileExtension(const std::string& fileName);
    std::string getFileNameWithoutExtension(const std::string& fileName);
    void setConsoleUTF8();
    void pause();
};