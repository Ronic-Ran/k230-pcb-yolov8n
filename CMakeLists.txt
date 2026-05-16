cmake_minimum_required(VERSION 3.10)
project(pcb_yolov8n)

set(CMAKE_CXX_STANDARD 14)

# 如果外部没传 K230_SDK 路径，就用默认值
if(NOT DEFINED K230_SDK)
    set(K230_SDK "/tmp/canmv")
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${K230_SDK}/src/big/nncase/include
    ${K230_SDK}/src/big/nncase/runtime
)

link_directories(
    ${K230_SDK}/src/big/nncase/lib
)

add_executable(pcb_yolov8n.elf
    main.cc
    yolov8n_pcb.cc
)

target_link_libraries(pcb_yolov8n.elf
    nncase
    nncase_rt_modules_k230
    pthread
)