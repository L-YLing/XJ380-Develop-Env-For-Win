#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <sstream>
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

namespace fs = std::filesystem;
using namespace std;

int download_files() {
    auto current_path_value = fs::current_path();
    
    // 使用 ANSI 字符串（不需要 L 或 wstring）
    std::string url = "https://gitee.com/GuoqiFish/"
                      "xingji-interactive-software-download-item-storage/"
                      "releases/download/20260519-1/XJ380_XACT_2026v4_xj380.zip";
    
    std::string savePath = current_path_value.string() + "\\bin\\XJ380_XACT_2026v4_xj380.zip";  // 加上文件名
    
    // 直接使用 URLDownloadToFileA，传入 const char*
    HRESULT hr = URLDownloadToFileA(
        NULL,
        url.c_str(),                // 直接传入 const char*
        savePath.c_str(),           // 直接传入 const char*
        0,
        NULL
    );
    
    if (SUCCEEDED(hr)) {
        std::cout << "下载成功！保存位置: " << savePath << std::endl;
    } else {
        std::cout << "下载失败，错误码: " << hr << std::endl;
    }
    
    return 0;
}

// 递归复制整个目录
void copyDirectory(const fs::path& source, const fs::path& target) {
    // 创建目标目录
    fs::create_directories(target);
    
    // 遍历源目录中的所有内容
    for (const auto& entry : fs::recursive_directory_iterator(source)) {
        const auto& path = entry.path();
        auto relative_path = fs::relative(path, source);
        auto target_path = target / relative_path;
        
        if (fs::is_directory(path)) {
            fs::create_directories(target_path);
        } else if (fs::is_regular_file(path)) {
            fs::copy_file(path, target_path, fs::copy_options::overwrite_existing);
        }
    }
}


int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    int current_path_confirm;
    auto current_path_value = fs::current_path();
    
    cout << "欢迎使用XJ380环境自动构建程序" << endl;
    cout << "当前工作目录: " << current_path_value.string() << endl;
    cout << "正在从网络下载XJ380编译套件" << endl;
    download_files();
    cout << "输入1进行自动安装/输入2以自定义安装：" << endl;
    cin >> current_path_confirm;

    if (current_path_confirm == 1) {
        try {
            fs::path source = current_path_value.string() + "\\bin\\";
            fs::path target = "D:\\bings";
            
            cout << "安装目录：";
            cin >> target;

            // 1. 复制整个目录
            std::cout << "正在复制文件..." << std::endl;
            copyDirectory(source, target);
            
            // 2. 删除原目录
            std::cout << "正在删除原目录..." << std::endl;
            fs::remove_all(source);
            
            std::cout << "目录移动成功！" << std::endl;
            
        } catch (const fs::filesystem_error& e) {
            std::cerr << "操作失败: " << e.what() << std::endl;
        }
        system("assoc .xc=xxcc.source");
        system("assoc .xxc=xxcc.source");
        system("assoc .xcc=xxcc.source");
        system("assoc .xxcc=xxcc.source");
        
        // 使用 stringstream 构建命令
        //ostringstream oss;
        //oss << "ftype xxcc.source=" 
        //    << current_path_value.string() 
        //    << "\\bin\\xxcc.exe"
        //    << " \"%1\" -o \"%~n1.exe\"";
        
        //system(oss.str().c_str());
        
        cout << "安装完成" << endl;
        Sleep(3000);
    } else if (current_path_confirm == 2) {
        cout << "自定义安装功能待实现" << endl;
        Sleep(3000);
    } else {
        cout << "请输入有效值1/2" << endl;
        Sleep(3000);
    }
    
    system("cls");
    Sleep(3000);
    return 0;
}