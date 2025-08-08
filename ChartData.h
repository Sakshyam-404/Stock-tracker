#pragma once
#include <string>

struct PieSlice {
    std::string label;
    float value;
    float percentage;
    int color;
    
    PieSlice(const std::string& l, float v, int c) 
        : label(l), value(v), percentage(0.0f), color(c) {}
};

struct BarData {
    std::string label;
    float value;
    float percentage;
    int color;
    
    BarData(const std::string& l, float v, int c) 
        : label(l), value(v), percentage(0.0f), color(c) {}
};

