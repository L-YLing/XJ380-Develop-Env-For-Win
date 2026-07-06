#include "downloader.h"
#include "utils.h"
#include <windows.h>
#include <winhttp.h>
#include <fstream>
#include <iostream>

#pragma comment(lib, "winhttp.lib")

bool WinHTTPDownloader::download(const std::string& url, const std::string& savePath) {
    std::wstring wUrl(url.begin(), url.end());
    std::wstring wSavePath(savePath.begin(), savePath.end());
    
    HINTERNET hSession = WinHttpOpen(L"XJ380-Env-Builder/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
                                     WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::cerr << "WinHttpOpen 失败" << std::endl;
        return false;
    }
    
    URL_COMPONENTS urlComp = {0};
    urlComp.dwStructSize = sizeof(URL_COMPONENTS);
    wchar_t hostName[256] = {0};
    wchar_t urlPath[1024] = {0};
    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = sizeof(hostName) / sizeof(wchar_t);
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = sizeof(urlPath) / sizeof(wchar_t);
    
    if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.length(), 0, &urlComp)) {
        std::cerr << "WinHttpCrackUrl 失败" << std::endl;
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    HINTERNET hConnect = WinHttpConnect(hSession, hostName, urlComp.nPort, 0);
    if (!hConnect) {
        std::cerr << "WinHttpConnect 失败" << std::endl;
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    DWORD flags = (urlComp.nPort == INTERNET_DEFAULT_HTTPS_PORT) ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlPath, NULL, WINHTTP_NO_REFERER, 
                                            WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!hRequest) {
        std::cerr << "WinHttpOpenRequest 失败" << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    DWORD maxRedirects = 10;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_REDIRECT_POLICY, &maxRedirects, sizeof(DWORD));
    
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        std::cerr << "WinHttpSendRequest 失败" << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        std::cerr << "WinHttpReceiveResponse 失败" << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            NULL, &statusCode, &statusSize, NULL)) {
        if (statusCode != 200) {
            std::cerr << "HTTP 请求失败，状态码: " << statusCode << std::endl;
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }
    }
    
    DWORD contentLength = 0;
    DWORD lengthSize = sizeof(contentLength);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, 
                        NULL, &contentLength, &lengthSize, NULL);
    
    std::ofstream outFile(wSavePath, std::ios::binary);
    if (!outFile) {
        std::cerr << "创建文件失败: " << savePath << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    char buffer[8192];
    DWORD bytesRead = 0;
    DWORD totalRead = 0;
    
    while (WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        outFile.write(buffer, bytesRead);
        totalRead += bytesRead;
        
        if (contentLength > 0 && progressCallback_) {
            double progress = (double)totalRead / contentLength * 100.0;
            progressCallback_(progress);
        }
    }
    
    outFile.close();
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    
    if (contentLength > 0 && totalRead != contentLength) {
        std::cerr << "下载不完整，期望: " << contentLength << " 字节，实际: " << totalRead << " 字节" << std::endl;
        utils::deleteFile(savePath);
        return false;
    }
    
    if (totalRead < 1024) {
        std::cerr << "下载的文件太小，可能是错误页面" << std::endl;
        utils::deleteFile(savePath);
        return false;
    }
    
    std::ifstream checkFile(savePath, std::ios::binary);
    if (checkFile) {
        unsigned char magic[4] = {0};
        checkFile.read((char*)magic, 4);
        if (magic[0] != 0x50 || magic[1] != 0x4B || magic[2] != 0x03 || magic[3] != 0x04) {
            std::cerr << "下载的文件不是有效的ZIP格式" << std::endl;
            checkFile.close();
            utils::deleteFile(savePath);
            return false;
        }
        checkFile.close();
    }
    
    return true;
}

void WinHTTPDownloader::setProgressCallback(std::function<void(double)> callback) {
    progressCallback_ = callback;
}

std::string getFileNameFromUrl(const std::string& url) {
    size_t lastSlash = url.find_last_of('/');
    if (lastSlash == std::string::npos) {
        return url;
    }
    return url.substr(lastSlash + 1);
}