#include "yolov8n_pcb.h"
#include <cmath>
#include <algorithm>

static bool det_cmp(const PcbDetection& a, const PcbDetection& b) {
    return a.score > b.score;
}

static float box_iou(const PcbDetection& a, const PcbDetection& b) {
    float x1 = std::max(a.x - a.w/2.0f, b.x - b.w/2.0f);
    float y1 = std::max(a.y - a.h/2.0f, b.y - b.h/2.0f);
    float x2 = std::min(a.x + a.w/2.0f, b.x + b.w/2.0f);
    float y2 = std::min(a.y + a.h/2.0f, b.y + b.h/2.0f);
    float inter = std::max(0.0f, x2-x1) * std::max(0.0f, y2-y1);
    return inter / (a.w*a.h + b.w*b.h - inter + 1e-6f);
}

void decode_yolov8_output(float* output, std::vector<PcbDetection>& results) {
    std::vector<PcbDetection> dets;
    
    for (int i = 0; i < YOLO_ANCHORS; i++) {
        float cx = output[0 * YOLO_ANCHORS + i];
        float cy = output[1 * YOLO_ANCHORS + i];
        float w  = output[2 * YOLO_ANCHORS + i];
        float h  = output[3 * YOLO_ANCHORS + i];
        
        float best_s = -1e9;
        int best_c = -1;
        for (int c = 0; c < YOLO_CLASSES; c++) {
            float s = output[(4 + c) * YOLO_ANCHORS + i];
            if (s > best_s) { best_s = s; best_c = c; }
        }
        if (best_s > CONF_THRESH) {
            dets.push_back({cx, cy, w, h, best_s, best_c});
        }
    }
    
    nms_boxes(dets, results);
}

void nms_boxes(std::vector<PcbDetection>& dets, std::vector<PcbDetection>& out) {
    std::sort(dets.begin(), dets.end(), det_cmp);
    std::vector<bool> sup(dets.size(), false);
    for (size_t i = 0; i < dets.size() && out.size() < YOLO_MAX_DET; i++) {
        if (sup[i]) continue;
        out.push_back(dets[i]);
        for (size_t j = i + 1; j < dets.size(); j++) {
            if (sup[j] || dets[j].class_id != dets[i].class_id) continue;
            if (box_iou(dets[i], dets[j]) > NMS_THRESH) sup[j] = true;
        }
    }
}
