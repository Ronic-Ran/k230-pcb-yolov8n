#include <stdint.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <vector>
#include "yolov8n_pcb.h"

// 图像缩放：最近邻算法
void resize_rgb(uint8_t* src, int sw, int sh, uint8_t* dst, int dw, int dh) {
    float sx = (float)sw / dw;
    float sy = (float)sh / dh;
    for (int y = 0; y < dh; y++) {
        for (int x = 0; x < dw; x++) {
            int sx_i = (int)(x * sx);
            int sy_i = (int)(y * sy);
            int s = (sy_i * sw + sx_i) * 3;
            int d = (y * dw + x) * 3;
            dst[d] = src[s];
            dst[d+1] = src[s+1];
            dst[d+2] = src[s+2];
        }
    }
}

// HWC 转 NCHW
void hwc2chw(uint8_t* hwc, uint8_t* chw, int h, int w) {
    for (int c = 0; c < 3; c++) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                chw[c * h * w + y * w + x] = hwc[(y * w + x) * 3 + c];
            }
        }
    }
}

int main(int argc, char** argv) {
    printf("=== K230 PCB Defect Detection ===\n");
    
    // 模型路径
    const char* kmodel_path = "/mnt/sdcard/best_yolov8n_320.kmodel";
    printf("Loading model: %s\n", kmodel_path);
    
    // 加载模型（这里先注释掉实际加载，等SDK环境确认后再打开）
    // YOLOv8nPCB detector(kmodel_path, 1);
    
    // 缓冲区定义
    uint8_t cam[640 * 480 * 3];
    uint8_t small[320 * 320 * 3];
    uint8_t input[1 * 3 * 320 * 320];
    
    printf("Entering main loop...\n");
    
    while (1) {
        // TODO: 摄像头采集
        // camera_capture(cam);
        
        // 预处理
        resize_rgb(cam, 640, 480, small, 320, 320);
        hwc2chw(small, input, 320, 320);
        
        // TODO: KPU推理
        // std::vector<PcbDetection> results;
        // detector.run(input, results);
        
        // 假数据测试编译
        std::vector<PcbDetection> results;
        
        printf("Detected %zu defects\n", results.size());
        for (size_t i = 0; i < results.size(); i++) {
            printf("  [%s] %.2f @ (%.2f, %.2f)\n", 
                   CLASS_NAMES[results[i].class_id], 
                   results[i].score, 
                   results[i].x, 
                   results[i].y);
        }
        
        usleep(50000);
    }
    
    return 0;
}
