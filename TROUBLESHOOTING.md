# K230 PCB YOLOv8n 链接错误解决方案

## 问题描述
编译失败，提示找不到库文件：
```
/usr/bin/ld: cannot find -lnncase
/usr/bin/ld: cannot find -lnncase_rt_modules_k230
```

## 根本原因
K230 SDK 中的 nncase 库文件没有被编译或者库文件路径配置不正确。

## 解决方案

### 方案 1：更新 GitHub Actions 工作流（推荐）

我已经为你修复了 `.github/workflows/build.yml` 文件，关键改进包括：

1. **添加 SDK 编译步骤**
   ```bash
   # 检查并编译 nncase 库
   if [ ! -f 'src/big/nncase/lib/libnncase.a' ] && [ -f 'src/big/nncase/CMakeLists.txt' ]; then
       mkdir -p src/big/nncase/build
       cd src/big/nncase/build
       cmake .. -DCMAKE_BUILD_TYPE=Release
       make -j4
   fi
   ```

2. **改进错误处理**
   - 添加 `set -e` 确保任何失败都会停止脚本
   - 添加调试输出检查库文件

3. **验证输出**
   - 在上传前验证 ELF 文件存在

### 方案 2：本地调试

如果在本地构建，按照以下步骤：

```bash
# 1. 克隆完整的 K230 SDK（包含所有子模块）
git clone --recursive https://github.com/kendryte/k230_canmv.git /tmp/canmv
cd /tmp/canmv

# 2. 更新所有子模块
git submodule update --init --recursive

# 3. 检查 nncase 库是否存在
ls -la src/big/nncase/lib/

# 如果不存在，尝试编译 nncase
if [ ! -d "src/big/nncase/lib" ]; then
    mkdir -p src/big/nncase/build
    cd src/big/nncase/build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
    cd /tmp/canmv
fi

# 4. 复制你的代码
cp -r your_project_files/* src/reference/ai_poc/pcb_yolov8n/

# 5. 构建项目
cd src/reference/ai_poc/pcb_yolov8n
mkdir -p build && cd build
cmake -DK230_SDK=/tmp/canmv ..
make -j4
```

### 方案 3：改进 CMakeLists.txt

我已经更新了 `CMakeLists.txt` 文件以添加更好的调试信息：

```cmake
# 调试信息：检查库文件是否存在
if(EXISTS "${K230_SDK}/src/big/nncase/lib")
    file(GLOB NNCASE_LIBS "${K230_SDK}/src/big/nncase/lib/*")
    message(STATUS "Found nncase libraries:")
    foreach(LIB ${NNCASE_LIBS})
        message(STATUS "  - ${LIB}")
    endforeach()
else()
    message(WARNING "nncase lib directory not found: ${K230_SDK}/src/big/nncase/lib")
endif()
```

## 快速检查清单

- [ ] K230 SDK 是否正确克隆（包含 `--recursive` 标志）
- [ ] 子模块是否已更新（`git submodule update --init --recursive`）
- [ ] `${K230_SDK}/src/big/nncase/lib/` 目录是否存在
- [ ] 目录中是否有 `libnncase.a` 和相关的库文件
- [ ] CMake 的 `link_directories()` 路径是否正确

## 验证库文件

运行以下命令查看是否找到了库文件：

```bash
# 在构建过程中会输出这些信息
cmake -DK230_SDK=/tmp/canmv .. 2>&1 | grep -i "nncase"

# 或者直接检查
ls -la /tmp/canmv/src/big/nncase/lib/
```

## 如果仍然失败

1. 检查 K230 Docker 镜像是否包含预编译的库：
   ```bash
   docker run -it ghcr.io/kendryte/k230_sdk:latest find /tmp/canmv/src/big/nncase/lib/ -name "*nncase*"
   ```

2. 查看完整的 CMake 输出以获得更多线索：
   ```bash
   cmake -DK230_SDK=/tmp/canmv -DCMAKE_VERBOSE_MAKEFILE=ON ..
   make VERBOSE=1
   ```

3. 如果库文件确实不存在，可能需要从源代码构建 nncase SDK

## 相关资源

- K230 SDK: https://github.com/kendryte/k230_canmv
- nncase 项目: https://github.com/kendryte/nncase
