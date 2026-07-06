#include "source.h"
#include "utils.h"
#include <memory>

std::unique_ptr<ISource> SourceFactory::createSource(SourceType type) {
    switch (type) {
        case SourceType::OFFICIAL: {
            class OfficialSource : public ISource {
            public:
                SourceType getType() const override { return SourceType::OFFICIAL; }
                std::string getName() const override { return "官网"; }
                
                bool isAvailable() override { return true; }
                
                std::vector<Kit> getKits() override {
                    std::vector<Kit> kits;
                    kits.push_back({KitType::XACT_XJ380, "XJ380 应用程序编译套件（XACT for XJ380）", "2026v4", 
                        "https://gitee.com/GuoqiFish/xingji-interactive-software-download-item-storage/releases/download/20260519-1/XJ380_XACT_2026v4_xj380.zip", 
                        "XJ380_XACT_2026v4_xj380.zip", "xact_xj380"});
                    kits.push_back({KitType::XACT_WINDOWS, "XJ380 应用程序编译套件（XACT for Windows）", "2026v4", 
                        "https://gitee.com/GuoqiFish/xingji-interactive-software-download-item-storage/releases/download/20260519-1/XJ380_XACT_2026v4_windows.zip", 
                        "XJ380_XACT_2026v4_windows.zip", "xact_windows"});
                    kits.push_back({KitType::API_STATIC, "XJ380 API 静态链接库及头文件（C/C++）", "2026v4", 
                        "https://gitee.com/GuoqiFish/xingji-interactive-software-download-item-storage/releases/download/20260519-1/XJ380_API_2026v4_static.zip", 
                        "XJ380_API_2026v4_static.zip", "api_static"});
                    return kits;
                }
            };
            return std::make_unique<OfficialSource>();
        }
        case SourceType::GITHUB: {
            class GitHubSource : public ISource {
            public:
                SourceType getType() const override { return SourceType::GITHUB; }
                std::string getName() const override { return "GitHub"; }
                
                bool isAvailable() override { return true; }
                
                std::vector<Kit> getKits() override {
                    std::vector<Kit> kits;
                    kits.push_back({KitType::XACT_XJ380, "XJ380 应用程序编译套件（XACT for XJ380）", "2026v4", 
                        "https://github.com/xingji-studio/XXCC-suite/releases/download/2026v4/XJ380_XACT_2026v4_xj380.zip", 
                        "XJ380_XACT_2026v4_xj380.zip", "xact_xj380"});
                    kits.push_back({KitType::XACT_WINDOWS, "XJ380 应用程序编译套件（XACT for Windows）", "2026v4", 
                        "https://github.com/xingji-studio/XXCC-suite/releases/download/2026v4/XJ380_XACT_2026v4_windows.zip", 
                        "XJ380_XACT_2026v4_windows.zip", "xact_windows"});
                    return kits;
                }
            };
            return std::make_unique<GitHubSource>();
        }
        case SourceType::OFFLINE: {
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
                    
                    auto checkAndAdd = [&](const std::string& filename, KitType type, const std::string& name, const std::string& targetDir) {
                        std::string path = utils::combinePaths(kitsDir, filename);
                        if (utils::fileExists(path)) {
                            kits.push_back({type, name, "offline", path, filename, targetDir});
                        }
                    };
                    
                    checkAndAdd("XACT_XJ380.zip", KitType::XACT_XJ380, "XJ380 应用程序编译套件（XACT for XJ380）", "xact_xj380");
                    checkAndAdd("XACT_Windows.zip", KitType::XACT_WINDOWS, "XJ380 应用程序编译套件（XACT for Windows）", "xact_windows");
                    checkAndAdd("XJ380_API_static.zip", KitType::API_STATIC, "XJ380 API 静态链接库及头文件（C/C++）", "api_static");
                    checkAndAdd("xswl.zip", KitType::XSWL, "XJ380 跨平台兼容层（XSWL）", "xswl");
                    
                    return kits;
                }
            };
            return std::make_unique<OfflineSource>();
        }
        default:
            return nullptr;
    }
}

std::string sourceTypeToString(SourceType type) {
    switch (type) {
        case SourceType::OFFICIAL: return "官网";
        case SourceType::GITHUB: return "GitHub";
        case SourceType::OFFLINE: return "离线";
        default: return "未知";
    }
}

SourceType stringToSourceType(const std::string& str) {
    if (str == "官网" || str == "official") return SourceType::OFFICIAL;
    if (str == "GitHub" || str == "github") return SourceType::GITHUB;
    if (str == "离线" || str == "offline") return SourceType::OFFLINE;
    return SourceType::OFFICIAL;
}