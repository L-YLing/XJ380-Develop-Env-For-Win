#include "include/source.h"
#include "include/kit.h"
#include "include/downloader.h"
#include "include/installer.h"
#include "include/utils.h"
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <limits>

std::string readLine(const std::string& prompt, const std::string& defaultValue = "") {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) {
        return defaultValue;
    }
    return input;
}

void printMenu() {
    std::cout << "\n=============================================" << std::endl;
    std::cout << "        XJ380 环境自动构建工具 v1.0" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "1. 使用官网下载源" << std::endl;
    std::cout << "2. 使用 GitHub 下载源" << std::endl;
    std::cout << "3. 使用离线安装包" << std::endl;
    std::cout << "4. 安装 XSWL 跨平台兼容层" << std::endl;
    std::cout << "5. 退出" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "请选择操作 [1-5]: ";
}

void printKits(const std::vector<Kit>& kits) {
    std::cout << "\n可用套件列表:" << std::endl;
    for (size_t i = 0; i < kits.size(); ++i) {
        std::cout << i + 1 << ". " << kits[i].name << std::endl;
        std::cout << "   版本: " << kits[i].version << std::endl;
        std::cout << "   目标目录: " << kits[i].targetDir << std::endl;
    }
}

bool downloadAndInstall(std::unique_ptr<ISource>& source, const Kit& kit, const std::string& installDir) {
    WinHTTPDownloader downloader;
    
    downloader.setProgressCallback([](double progress) {
        std::cout << "\r下载进度: " << static_cast<int>(progress) << "%";
        std::cout.flush();
    });
    
    std::string downloadPath = utils::combinePaths(utils::getTempDirectory(), kit.fileName);
    
    std::cout << "\n正在下载: " << kit.name << std::endl;
    std::cout << "来源: " << kit.url << std::endl;
    std::cout << "保存到: " << downloadPath << std::endl;
    
    if (!downloader.download(kit.url, downloadPath)) {
        std::cerr << "\n下载失败！" << std::endl;
        return false;
    }
    
    std::cout << "\n下载完成！" << std::endl;
    
    Installer installer(installDir);
    return installer.installKit(kit, downloadPath);
}

void installFromSource(SourceType sourceType) {
    auto source = SourceFactory::createSource(sourceType);
    if (!source) {
        std::cerr << "创建下载源失败！" << std::endl;
        return;
    }
    
    if (!source->isAvailable()) {
        std::cerr << source->getName() << " 源不可用！" << std::endl;
        return;
    }
    
    auto kits = source->getKits();
    if (kits.empty()) {
        std::cerr << "没有可用的套件！" << std::endl;
        return;
    }
    
    printKits(kits);
    
    std::string selection = readLine("\n请选择要安装的套件 (输入序号，多个用逗号分隔，全部安装输入0): ", "0");
    std::string installDir = readLine("请输入安装目录 (默认: D:\\XJ380): ", "D:\\XJ380");
    
    int successCount = 0;
    int failCount = 0;
    
    if (selection == "0") {
        for (const auto& kit : kits) {
            if (downloadAndInstall(source, kit, installDir)) {
                successCount++;
            } else {
                failCount++;
            }
        }
    } else {
        size_t pos = 0;
        std::string token;
        while ((pos = selection.find(",")) != std::string::npos) {
            token = selection.substr(0, pos);
            try {
                int index = std::stoi(token) - 1;
                if (index >= 0 && index < static_cast<int>(kits.size())) {
                    if (downloadAndInstall(source, kits[index], installDir)) {
                        successCount++;
                    } else {
                        failCount++;
                    }
                }
            } catch (...) {
                std::cerr << "无效的套件序号: " << token << std::endl;
                failCount++;
            }
            selection.erase(0, pos + 1);
        }
        try {
            int index = std::stoi(selection) - 1;
            if (index >= 0 && index < static_cast<int>(kits.size())) {
                if (downloadAndInstall(source, kits[index], installDir)) {
                    successCount++;
                } else {
                    failCount++;
                }
            }
        } catch (...) {
            std::cerr << "无效的套件序号: " << selection << std::endl;
            failCount++;
        }
    }
    
    std::cout << "\n安装结果: " << successCount << " 成功, " << failCount << " 失败" << std::endl;
    if (failCount > 0) {
        std::cerr << "部分套件安装失败，请检查错误信息！" << std::endl;
    }
}

void installXSWL() {
    std::cout << "\n安装 XJ380 跨平台兼容层 (XSWL)" << std::endl;
    
    std::string installDir = readLine("请输入安装目录 (默认: D:\\XJ380): ", "D:\\XJ380");
    
    Kit xswlKit = {
        KitType::XSWL,
        "XJ380 跨平台兼容层（XSWL）",
        "latest",
        "https://github.com/xingji-studio/xswl/archive/refs/heads/main.zip",
        "xswl_main.zip",
        "xswl"
    };
    
    auto source = SourceFactory::createSource(SourceType::OFFLINE);
    if (source->isAvailable()) {
        auto offlineKits = source->getKits();
        for (const auto& kit : offlineKits) {
            if (kit.type == KitType::XSWL) {
                std::cout << "检测到离线安装包，使用离线模式..." << std::endl;
                Installer installer(installDir);
                installer.installFromOffline(kit);
                return;
            }
        }
    }
    
    WinHTTPDownloader downloader;
    downloader.setProgressCallback([](double progress) {
        std::cout << "\r下载进度: " << static_cast<int>(progress) << "%";
        std::cout.flush();
    });
    
    std::string downloadPath = utils::combinePaths(utils::getTempDirectory(), xswlKit.fileName);
    
    std::cout << "\n正在下载: " << xswlKit.name << std::endl;
    std::cout << "来源: " << xswlKit.url << std::endl;
    
    if (!downloader.download(xswlKit.url, downloadPath)) {
        std::cerr << "\n下载失败！" << std::endl;
        return;
    }
    
    std::cout << "\n下载完成！" << std::endl;
    
    Installer installer(installDir);
    installer.installKit(xswlKit, downloadPath);
    
    std::cout << "\nXSWL 安装完成！" << std::endl;
}

int main() {
    utils::setConsoleUTF8();
    
    std::cout << "欢迎使用 XJ380 环境自动构建工具！" << std::endl;
    std::cout << "当前工作目录: " << utils::getCurrentDirectory() << std::endl;
    
    int choice = 0;
    do {
        printMenu();
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\n无效输入，请输入数字！" << std::endl;
            utils::pause();
            system("cls");
            continue;
        }
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                std::cout << "\n使用官网下载源..." << std::endl;
                installFromSource(SourceType::OFFICIAL);
                break;
            case 2:
                std::cout << "\n使用 GitHub 下载源..." << std::endl;
                installFromSource(SourceType::GITHUB);
                break;
            case 3:
                std::cout << "\n使用离线安装包..." << std::endl;
                installFromSource(SourceType::OFFLINE);
                break;
            case 4:
                installXSWL();
                break;
            case 5:
                std::cout << "\n退出程序..." << std::endl;
                break;
            default:
                std::cout << "\n无效选择，请输入 1-5！" << std::endl;
                break;
        }
        
        if (choice != 5) {
            utils::pause();
            system("cls");
        }
        
    } while (choice != 5);
    
    return 0;
}