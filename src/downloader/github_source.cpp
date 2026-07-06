#include "source.h"
#include "kit.h"
#include "utils.h"
#include "downloader.h"
#include "../third_party/nlohmann/json.hpp"
#include <string>
#include <vector>
#include <memory>

using json = nlohmann::json;

class GitHubSource : public ISource {
public:
    SourceType getType() const override { return SourceType::GITHUB; }
    std::string getName() const override { return "GitHub"; }
    
    bool isAvailable() override {
        return true;
    }
    
    std::vector<Kit> getKits() override {
        std::vector<Kit> kits;
        
        kits.push_back({
            KitType::XACT_WINDOWS,
            "XJ380 应用程序编译套件（XACT for Windows）",
            "latest",
            "https://github.com/xingji-studio/XXCC-suite/releases/latest/download/XACT_Windows_latest.zip",
            "XACT_Windows_latest.zip",
            "xact_windows"
        });
        
        return kits;
    }
};

std::unique_ptr<ISource> createGitHubSource() {
    return std::make_unique<GitHubSource>();
}