#ifndef YOLOV8N_PCB_H
#define YOLOV8N_PCB_H

#include <vector>
#include <cstdint>

#define YOLO_CLASSES     6
#define YOLO_ANCHORS     2100
#define YOLO_MAX_DET     50
#define CONF_THRESH      0.30f
#define NMS_THRESH       0.45f

static const char* CLASS_NAMES[YOLO_CLASSES] = {
    "Missing_hole", "Mouse_bite", "Open_circuit",
    "Short", "Spur", "Spurious_copper"
};

struct PcbDetection {
    float x, y, w, h;
    float score;
    int   class_id;
};

void decode_yolov8_output(float* output, std::vector<PcbDetection>& results);
void nms_boxes(std::vector<PcbDetection>& dets, std::vector<PcbDetection>& out);

#endif
