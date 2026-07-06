#define UNICODE
#define _UNICODE
#include "installer.h"
#include "utils.h"
#include <windows.h>
#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

Installer::Installer(const std::string& baseDir) : baseDir_(baseDir) {
    if (!utils::directoryExists(baseDir_)) {
        utils::createDirectory(baseDir_);
    }
}

bool Installer::installKit(const Kit& kit, const std::string& downloadPath) {
    std::string targetDir = utils::combinePaths(baseDir_, kit.targetDir);
    
    if (!utils::directoryExists(targetDir)) {
        utils::createDirectory(targetDir);
    }
    
    std::cout << "\n开始安装: " << kit.name << std::endl;
    std::cout << "目标目录: " << targetDir << std::endl;
    
    if (!extractZipWithProgress(downloadPath, targetDir)) {
        std::cerr << "解压失败: " << downloadPath << std::endl;
        return false;
    }
    
    std::cout << "\n安装完成: " << kit.name << std::endl;
    
    if (kit.type == KitType::XACT_WINDOWS || kit.type == KitType::XACT_XJ380) {
        createAssociations();
        setupEnvironment();
    }
    
    return true;
}

bool Installer::installFromOffline(const Kit& kit) {
    return installKit(kit, kit.url);
}

bool Installer::extractZipWithProgress(const std::string& zipPath, const std::string& destDir) {
    if (!utils::directoryExists(destDir)) {
        if (!utils::createDirectory(destDir)) {
            std::cerr << "创建目标目录失败: " << destDir << std::endl;
            return false;
        }
    }
    
    std::cout << "\n正在解压: " << zipPath << std::endl;
    std::cout << "目标目录: " << destDir << std::endl;
    std::cout << "解压中，请稍候..." << std::endl;
    
    std::string command = "powershell -Command \"$zip = Get-ChildItem '" + zipPath + "'; $total = 0; $zip | Expand-Archive -DestinationPath '" + destDir + "' -Force; Write-Host '解压完成'\"";
    
    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    PROCESS_INFORMATION pi = {0};
    
    std::wstring wCommand(command.begin(), command.end());
    if (!CreateProcess(NULL, const_cast<LPWSTR>(wCommand.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "创建解压进程失败" << std::endl;
        return false;
    }
    
    DWORD waitResult;
    int progress = 0;
    const char* anim = "|/-\\";
    
    do {
        waitResult = WaitForSingleObject(pi.hProcess, 500);
        
        progress = (progress + 1) % 100;
        int animIndex = progress % 4;
        
        std::cout << "\r[";
        for (int i = 0; i < 20; ++i) {
            if (i < progress / 5) {
                std::cout << "=";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "] " << progress << "% " << anim[animIndex];
        std::cout.flush();
        
    } while (waitResult == WAIT_TIMEOUT);
    
    std::cout << "\r[====================] 100% 完成" << std::endl;
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    if (exitCode != 0) {
        std::cerr << "解压失败，退出码: " << exitCode << std::endl;
        return false;
    }
    
    std::cout << "解压成功！" << std::endl;
    return true;
}

bool Installer::extractZip(const std::string& zipPath, const std::string& destDir) {
    return extractZipWithProgress(zipPath, destDir);
}

bool Installer::copyDirectory(const std::string& source, const std::string& target) {
    try {
        if (fs::exists(target)) {
            fs::remove_all(target);
        }
        fs::copy(source, target, fs::copy_options::recursive);
        return true;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "复制目录失败: " << e.what() << std::endl;
        return false;
    }
}

bool Installer::createAssociations() {
    system("assoc .xc=xxcc.source >nul 2>&1");
    system("assoc .xxc=xxcc.source >nul 2>&1");
    system("assoc .xcc=xxcc.source >nul 2>&1");
    system("assoc .xxcc=xxcc.source >nul 2>&1");
    
    std::string xxccPath = utils::combinePaths(baseDir_, "xact_windows\\bin\\xxcc.exe");
    if (utils::fileExists(xxccPath)) {
        std::string command = "ftype xxcc.source=\"" + xxccPath + "\" \"%1\" -o \"%~n1.exe\"";
        system(command.c_str());
    }
    
    return true;
}

bool Installer::setupEnvironment() {
    std::string binPath = utils::combinePaths(baseDir_, "xact_windows\\bin");
    
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Environment", 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
        WCHAR pathBuffer[32767] = {0};
        DWORD pathSize = sizeof(pathBuffer);
        
        if (RegQueryValueEx(hKey, L"Path", NULL, NULL, (LPBYTE)pathBuffer, &pathSize) == ERROR_SUCCESS) {
            std::wstring currentPath(pathBuffer);
            std::wstring wBinPath(binPath.begin(), binPath.end());
            
            if (currentPath.find(wBinPath) == std::wstring::npos) {
                std::wstring newPath = currentPath + L";" + wBinPath;
                RegSetValueEx(hKey, L"Path", 0, REG_EXPAND_SZ, (const BYTE*)newPath.c_str(), 
                              (DWORD)(newPath.length() + 1) * sizeof(WCHAR));
            }
        }
        
        RegCloseKey(hKey);
        SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", 
                          SMTO_ABORTIFHUNG, 5000, NULL);
    }
    
    return true;
}