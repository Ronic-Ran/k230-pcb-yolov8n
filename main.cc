#include "yolov8n_pcb.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>

// 图像缩放：最近邻算法，快速
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
    for (int c = 0; c < 3; c++)
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
                chw[c * h * w + y * w + x] = hwc[(y * w + x) * 3 + c];
}

int main(int argc, char** argv) {
    printf("=== K230 PCB Defect Detection ===\n");
    
    // 模型路径（SD卡根目录）
    const char* kmodel_path = "/mnt/sdcard/best_yolov8n_320.kmodel";
    
    // 加载模型（这里假设 ai_base 已经帮你封装好了，如果没有，后面再加）
    // TODO: 根据实际 SDK 的 KPU 加载方式调整
    printf("Loading model: %s\n", kmodel_path);
    
    // 缓冲区
    uint8_t cam[640 * 480 * 3];       // 摄像头原始图 640x480 RGB
    uint8_t small[320 * 320 * 3];     // 缩放后 320x320
    uint8_t input[1 * 3 * 320 * 320]; // 模型输入 NCHW
    
    // TODO: 初始化摄像头和屏幕（根据你的 SDK 示例代码添加）
    
    while (1) {
        // TODO: 采集一帧到 cam 数组
        // camera_capture(cam);
        
        // 预处理
        resize_rgb(cam, 640, 480, small, 320, 320);
        hwc2chw(small, input, 320, 320);
        
        // TODO: KPU 推理
        // 1. 把 input 传给 KPU
        // 2. 获取输出 float* output
        // 3. 解码
        
        // 假输出（先测试编译能不能过）
        std::vector<PcbDetection> results;
        // decode_yolov8_output(output, results);
        
        printf("Detected %zu defects\n", results.size());
        for (auto& d : results) {
            printf("  [%s] %.2f @ (%.2f, %.2f)\n", 
                   CLASS_NAMES[d.class_id], d.score, d.x, d.y);
            
            // 坐标映射到 800x480 屏幕
            // int x1 = (int)((d.x - d.w/2) * 800);
            // int y1 = (int)((d.y - d.h/2) * 480);
            // int x2 = (int)((d.x + d.w/2) * 800);
            // int y2 = (int)((d.y + d.h/2) * 480);
            // draw_box(x1, y1, x2, y2, CLASS_NAMES[d.class_id], d.score);
        }
        
        usleep(50000); // 50ms
    }
    
    return 0;
}