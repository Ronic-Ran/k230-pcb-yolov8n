cmake_minimum_required(VERSION 3.10)
project(pcb_yolov8n)

set(CMAKE_CXX_STANDARD 14)

# 如果外部没传 K230_SDK 路径，就用默认值
if(NOT DEFINED K230_SDK)
    set(K230_SDK "/tmp/canmv")
endif()

message(STATUS "K230_SDK path: ${K230_SDK}")

# 设置库目录
set(NNCASE_LIB_DIR "${K230_SDK}/src/big/nncase/lib")

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${K230_SDK}/src/big/nncase/include
    ${K230_SDK}/src/big/nncase/runtime
)

link_directories(
    ${NNCASE_LIB_DIR}
)

# 调试信息：检查库文件是否存在
if(EXISTS "${NNCASE_LIB_DIR}")
    file(GLOB NNCASE_LIBS "${NNCASE_LIB_DIR}/*")
    message(STATUS "Found nncase lib directory")
    message(STATUS "Libraries in ${NNCASE_LIB_DIR}:")
    foreach(LIB ${NNCASE_LIBS})
        get_filename_component(LIB_NAME ${LIB} NAME)
        message(STATUS "  - ${LIB_NAME}")
    endforeach()
else()
    message(WARNING "nncase lib directory not found: ${NNCASE_LIB_DIR}")
endif()

add_executable(pcb_yolov8n.elf
    main.cc
    yolov8n_pcb.cc
)

message(STATUS "Link directories: ${NNCASE_LIB_DIR}")
message(STATUS "Linking libraries: nncase, nncase_rt_modules_k230, pthread")

target_link_libraries(pcb_yolov8n.elf
    nncase
    nncase_rt_modules_k230
    pthread
)
