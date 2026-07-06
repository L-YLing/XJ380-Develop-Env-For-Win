#include "source.h"
#include "kit.h"
#include "utils.h"
#include <string>
#include <vector>
#include <memory>

class OfflineSource : public ISource {
public:
    SourceType getType() const override { return SourceType::OFFLINE; }
    std::string getName() const override { return "离线"; }
    
    bool isAvailable() override {
        std::string kitsDir = utils::combinePaths(utils::getCurrentDirectory(), "kits");
        return utils::directoryExists(kitsDir);
    }
    
    std::vector<Kit> getKits() override {
        std::vector<Kit> kits;
        std::string kitsDir = utils::combinePaths(utils::getCurrentDirectory(), "kits");
        
        if (!utils::directoryExists(kitsDir)) {
            return kits;
        }
        
        std::string xactXj380Path = utils::combinePaths(kitsDir, "XACT_XJ380.zip");
        if (utils::fileExists(xactXj380Path)) {
            kits.push_back({
                KitType::XACT_XJ380,
                "XJ380 应用程序编译套件（XACT for XJ380）",
                "offline",
                xactXj380Path,
                "XACT_XJ380.zip",
                "xact_xj380"
            });
        }
        
        std::string xactWindowsPath = utils::combinePaths(kitsDir, "XACT_Windows.zip");
        if (utils::fileExists(xactWindowsPath)) {
            kits.push_back({
                KitType::XACT_WINDOWS,
                "XJ380 应用程序编译套件（XACT for Windows）",
                "offline",
                xactWindowsPath,
                "XACT_Windows.zip",
                "xact_windows"
            });
        }
        
        std::string apiStaticPath = utils::combinePaths(kitsDir, "XJ380_API_static.zip");
        if (utils::fileExists(apiStaticPath)) {
            kits.push_back({
                KitType::API_STATIC,
                "XJ380 API 静态链接库及头文件（C/C++）",
                "offline",
                apiStaticPath,
                "XJ380_API_static.zip",
                "api_static"
            });
        }
        
        std::string xswlPath = utils::combinePaths(kitsDir, "xswl.zip");
        if (utils::fileExists(xswlPath)) {
            kits.push_back({
                KitType::XSWL,
                "XJ380 跨平台兼容层（XSWL）",
                "offline",
                xswlPath,
                "xswl.zip",
                "xswl"
            });
        }
        
        return kits;
    }
};

std::unique_ptr<ISource> createOfflineSource() {
    return std::make_unique<OfflineSource>();
}