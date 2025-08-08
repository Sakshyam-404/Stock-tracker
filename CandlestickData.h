#pragma once
#include <string>

struct CandlestickData {
    std::string date;
    float open;
    float high;
    float low;
    float close;
    float volume;
    
    CandlestickData(const std::string& d, float o, float h, float l, float c, float v)
        : date(d), open(o), high(h), low(l), close(c), volume(v) {}
}; 