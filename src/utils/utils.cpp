#include "utils.h"
#include <windows.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace utils {

bool fileExists(const std::string& path) {
    return fs::exists(path);
}

bool directoryExists(const std::string& path) {
    return fs::is_directory(path);
}

bool createDirectory(const std::string& path) {
    return fs::create_directories(path);
}

bool deleteFile(const std::string& path) {
    return fs::remove(path);
}

bool deleteDirectory(const std::string& path) {
    return fs::remove_all(path);
}

std::string getCurrentDirectory() {
    return fs::current_path().string();
}

std::string getTempDirectory() {
    return fs::temp_directory_path().string();
}

std::string combinePaths(const std::string& path1, const std::string& path2) {
    return (fs::path(path1) / path2).string();
}

std::string getFileExtension(const std::string& fileName) {
    return fs::path(fileName).extension().string();
}

std::string getFileNameWithoutExtension(const std::string& fileName) {
    return fs::path(fileName).stem().string();
}

void setConsoleUTF8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

void pause() {
    system("pause");
}

} // namespace utils