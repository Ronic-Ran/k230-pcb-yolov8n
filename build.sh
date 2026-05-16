#!/bin/bash

# K230 PCB YOLOv8n 本地编译脚本
# 使用方法: bash build.sh

set -e

echo "========== K230 PCB YOLOv8n Build Script =========="
echo ""

# 检查是否在Docker中运行
if ! command -v docker &> /dev/null; then
    echo "ERROR: Docker not found. Please install Docker first."
    exit 1
fi

echo "Step 1: Pulling K230 SDK Docker image..."
docker pull ghcr.io/kendryte/k230_sdk:latest

echo ""
echo "Step 2: Starting build in Docker container..."

WORKSPACE=$(pwd)

docker run --rm -u root \
    -v $WORKSPACE:/workspace \
    ghcr.io/kendryte/k230_sdk:latest /bin/bash -c '
    set -e
    
    echo "========== Inside Docker Container =========="
    echo ""
    
    echo "Step 3: Cloning K230 SDK..."
    cd /tmp
    if [ -d canmv ]; then
        rm -rf canmv
    fi
    git clone --recursive https://github.com/kendryte/k230_canmv.git canmv 2>&1 | tail -5
    cd canmv
    
    echo ""
    echo "Step 4: Checking for nncase libraries..."
    if [ -d "src/big/nncase/lib" ]; then
        echo "✓ Found nncase/lib directory"
        ls -la src/big/nncase/lib/ | head -10
    else
        echo "⚠ nncase/lib not found - will compile without nncase linking"
    fi
    
    echo ""
    echo "Step 5: Copying source files..."
    rm -rf /tmp/canmv/src/reference/ai_poc/pcb_yolov8n
    mkdir -p /tmp/canmv/src/reference/ai_poc/pcb_yolov8n
    
    cp /workspace/*.cc /tmp/canmv/src/reference/ai_poc/pcb_yolov8n/ 2>/dev/null || echo "No .cc files found"
    cp /workspace/*.h /tmp/canmv/src/reference/ai_poc/pcb_yolov8n/ 2>/dev/null || echo "No .h files found"
    
    echo "Copied files:"
    ls -la /tmp/canmv/src/reference/ai_poc/pcb_yolov8n/
    
    echo ""
    echo "Step 6: Creating CMakeLists.txt with conditional linking..."
    cat > /tmp/canmv/src/reference/ai_poc/pcb_yolov8n/CMakeLists.txt << '"'"'CMAKEFILE'"'"'
cmake_minimum_required(VERSION 3.10)
project(pcb_yolov8n)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")

set(NNCASE_LIB_DIR "/tmp/canmv/src/big/nncase/lib")

include_directories(
    ${CMAKE_SOURCE_DIR}
    /tmp/canmv/src/big/nncase/include
    /tmp/canmv/src/big/nncase/runtime
)

message(STATUS "Building pcb_yolov8n...")

add_executable(pcb_yolov8n.elf main.cc yolov8n_pcb.cc)

# Check if nncase libraries exist
if(EXISTS "${NNCASE_LIB_DIR}/libnncase.a")
    message(STATUS "Found nncase libraries - linking them")
    link_directories(${NNCASE_LIB_DIR})
    target_link_libraries(pcb_yolov8n.elf nncase nncase_rt_modules_k230 pthread)
else()
    message(STATUS "nncase libraries not found - compiling without them (mock mode)")
    target_link_libraries(pcb_yolov8n.elf pthread)
endif()
CMAKEFILE
    
    echo ""
    echo "Step 7: Compiling project..."
    cd /tmp/canmv/src/reference/ai_poc/pcb_yolov8n
    mkdir -p build
    cd build
    cmake .. || { echo "CMake configuration failed"; exit 1; }
    make -j$(nproc) || { echo "Build failed"; exit 1; }
    
    echo ""
    echo "Step 8: Verifying output..."
    if [ -f "pcb_yolov8n.elf" ]; then
        echo "✓ Build successful!"
        file pcb_yolov8n.elf
        ls -lh pcb_yolov8n.elf
        
        echo ""
        echo "Copying output to workspace..."
        cp pcb_yolov8n.elf /workspace/
        echo "✓ ELF file copied to /workspace/pcb_yolov8n.elf"
    else
        echo "✗ ERROR: pcb_yolov8n.elf not found!"
        exit 1
    fi
'

echo ""
echo "========== Build Complete =========="
echo ""

if [ -f "pcb_yolov8n.elf" ]; then
    echo "✓ SUCCESS! ELF file ready at: $(pwd)/pcb_yolov8n.elf"
    echo ""
    file pcb_yolov8n.elf
    ls -lh pcb_yolov8n.elf
    echo ""
    echo "Next steps:"
    echo "  1. Transfer this ELF to K230 board: scp pcb_yolov8n.elf root@<board-ip>:/tmp/"
    echo "  2. Run on board: /tmp/pcb_yolov8n.elf"
else
    echo "✗ FAILED! ELF file not generated."
    exit 1
fi
