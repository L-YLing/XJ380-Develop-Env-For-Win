#pragma once

#include <string>
#include <functional>

class IDownloader {
public:
    virtual ~IDownloader() = default;
    virtual bool download(const std::string& url, const std::string& savePath) = 0;
    virtual void setProgressCallback(std::function<void(double)> callback) = 0;
};

class WinHTTPDownloader : public IDownloader {
public:
    bool download(const std::string& url, const std::string& savePath) override;
    void setProgressCallback(std::function<void(double)> callback) override;

private:
    std::function<void(double)> progressCallback_;
};

std::string getFileNameFromUrl(const std::string& url);