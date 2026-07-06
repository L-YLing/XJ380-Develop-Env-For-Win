# 编译
**以下为推荐的编译项:**
| **编译器** | **标准** | **系统环境** |
| --- | --- | --- |
| **`clang++ 16.0`** | **`C++17`** | **`Windows7+`** |
> * **`clang++`版本必须在16.0以上**
> * **`C++`必须使用C++17或以上版本的标准**
> * **该程序仅能够在`Windows7`及以上版本的Windows下编译**

**构建命令：**
```bash
clang++ -std=c++17 main.cpp admin.res
```
```shell
clang++ -std=c++17 -Iinclude -Ithird_party main.cpp src/downloader/source_factory.cpp src/downloader/kit.cpp src/downloader/winhttp_downloader.cpp src/installer/installer.cpp src/utils/utils.cpp -o XJ380-Env-Builder.exe -lwinhttp -lshlwapi -lshell32 -ladvapi32 -luser32
```
**`admin.res`需要Visual Studio(从[VS官网](https://visualstudio.microsoft.com/)下载)编译`admin.rc`文件得到，编译命令为：**
```bash
rc admin.rc
```