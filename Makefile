directory = out_dist

all: clean build

build: ensure_dir
	@echo "开始编译..."
	clang++ -std=c++17 -Iinclude -Ithird_party main.cpp \
		src/downloader/source_factory.cpp \
		src/downloader/kit.cpp \
		src/downloader/winhttp_downloader.cpp \
		src/installer/installer.cpp src/utils/utils.cpp \
		-o $(directory)/XJ380-Env-Builder.exe \
		-lwinhttp -lshlwapi -lshell32 -ladvapi32 -luser32
	@echo "构建完成!"

ensure_dir:
	@echo "检查输出目录..."
	@if [ -d "$(directory)" ]; then \
		echo "目录已存在，跳过创建"; \
	else \
		echo "创建目录: $(directory)"; \
		mkdir -p $(directory); \
	fi

clean:
	@echo "清理构建产物中..."
	rm -rf $(directory)
	@echo "清理完成"

.PHONY: all build ensure_dir clean