#include "source.h"
#include "kit.h"
#include "utils.h"
#include "downloader.h"
#include <string>
#include <vector>
#include <regex>
#include <memory>

class OfficialSource : public ISource {
public:
    SourceType getType() const override { return SourceType::OFFICIAL; }
    std::string getName() const override { return "官网"; }
    
    bool isAvailable() override {
        return true;
    }
    
    std::vector<Kit> getKits() override {
        std::vector<Kit> kits;
        
        kits.push_back({
            KitType::XACT_XJ380,
            "XJ380 应用程序编译套件（XACT for XJ380）",
            "latest",
            "https://www.xingjisoft.com/os/xj380/download/XACT_XJ380_latest.zip",
            "XACT_XJ380_latest.zip",
            "xact_xj380"
        });
        
        kits.push_back({
            KitType::XACT_WINDOWS,
            "XJ380 应用程序编译套件（XACT for Windows）",
            "latest",
            "https://www.xingjisoft.com/os/xj380/download/XACT_Windows_latest.zip",
            "XACT_Windows_latest.zip",
            "xact_windows"
        });
        
        kits.push_back({
            KitType::API_STATIC,
            "XJ380 API 静态链接库及头文件（C/C++）",
            "latest",
            "https://www.xingjisoft.com/os/xj380/download/XJ380_API_static_latest.zip",
            "XJ380_API_static_latest.zip",
            "api_static"
        });
        
        return kits;
    }
};

std::unique_ptr<ISource> createOfficialSource() {
    return std::make_unique<OfficialSource>();
}