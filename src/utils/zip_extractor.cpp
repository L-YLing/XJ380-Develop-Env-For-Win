#include "utils.h"
#include <windows.h>
#include <shlwapi.h>
#include <iostream>

#pragma comment(lib, "shlwapi.lib")

bool extractZip(const std::string& zipPath, const std::string& destDir) {
    std::wstring wZipPath(zipPath.begin(), zipPath.end());
    std::wstring wDestDir(destDir.begin(), destDir.end());
    
    if (!utils::directoryExists(destDir)) {
        if (!utils::createDirectory(destDir)) {
            std::cerr << "创建目标目录失败: " << destDir << std::endl;
            return false;
        }
    }
    
    SHELLEXECUTEINFO sei = {0};
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = NULL;
    sei.lpVerb = L"extract";
    sei.lpFile = wZipPath.c_str();
    sei.lpParameters = L"\"*\" \"/e\" \"/o\"";
    sei.lpDirectory = wDestDir.c_str();
    sei.nShow = SW_HIDE;
    
    if (!ShellExecuteEx(&sei)) {
        std::cerr << "ShellExecuteEx 失败" << std::endl;
        return false;
    }
    
    if (sei.hProcess != NULL) {
        WaitForSingleObject(sei.hProcess, INFINITE);
        CloseHandle(sei.hProcess);
    }
    
    return true;
}