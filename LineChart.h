#pragma once
#include <FL/Fl_Widget.H>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "CandlestickData.h"

class LineChart : public Fl_Widget {
private:
    std::vector<CandlestickData> data;
    float minPrice, maxPrice;
    
public:
    LineChart(int x, int y, int w, int h, const char* label = 0) 
        : Fl_Widget(x, y, w, h, label) {
        minPrice = 0;
        maxPrice = 100;
    }
    
    void setData(const std::vector<CandlestickData>& newData) {
        data = newData;
        
        if (!data.empty()) {
            minPrice = data[0].close;
            maxPrice = data[0].close;
            
            for (const auto& point : data) {
                if (point.close < minPrice) minPrice = point.close;
                if (point.close > maxPrice) maxPrice = point.close;
            }
            
            float range = maxPrice - minPrice;
            minPrice -= range * 0.1f;
            maxPrice += range * 0.1f;
        }
        
        redraw();
    }
    
    void clear() {
        data.clear();
        redraw();
    }
    
protected:
    void draw() override {
        fl_color(FL_WHITE);
        fl_rectf(x(), y(), w(), h());
        
        if (data.empty()) {
            fl_color(FL_GRAY);
            fl_font(FL_HELVETICA, 14);
            fl_draw("No data available", x() + w()/2 - 50, y() + h()/2);
            return;
        }
        
        int chartX = x() + 60;
        int chartY = y() + 20;
        int chartW = w() - 80;
        int chartH = h() - 60;
        
        if (data.size() > 1) {
            fl_color(FL_GREEN);
            fl_line_style(FL_SOLID, 2);
            
            for (size_t i = 0; i < data.size() - 1; i++) {
                // Reverse the x-axis: oldest data on left, newest on right
                int x1 = chartX + (int)(data.size() - 1 - i) * chartW / (int)(data.size() - 1);
                int y1 = chartY + chartH - (int)((data[i].close - minPrice) / (maxPrice - minPrice) * chartH);
                int x2 = chartX + (int)(data.size() - 1 - (i + 1)) * chartW / (int)(data.size() - 1);
                int y2 = chartY + chartH - (int)((data[i + 1].close - minPrice) / (maxPrice - minPrice) * chartH);
                fl_line(x1, y1, x2, y2);
            }
            
            if (!data.empty()) {
                // Current point (newest) on the right
                int lastX = chartX + (int)(data.size() - 1 - (data.size() - 1)) * chartW / (int)(data.size() - 1);
                int lastY = chartY + chartH - (int)((data[0].close - minPrice) / (maxPrice - minPrice) * chartH);
                fl_color(FL_GREEN);
                fl_pie(lastX - 3, lastY - 3, 6, 6, 0, 360);
            }
        }
        
        fl_color(FL_BLACK);
        fl_font(FL_HELVETICA, 10);
        
        for (int i = 0; i <= 5; i++) {
            float price = maxPrice - (maxPrice - minPrice) * i / 5.0f;
            int yPos = chartY + (chartH * i) / 5;
            
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << price;
            fl_draw(oss.str().c_str(), x() + 5, yPos + 4);
        }
        
        int step = (int)data.size() / 4;
        if (step < 1) step = 1;
        
        for (size_t i = 0; i < data.size(); i += step) {
            // Reverse the x-axis for date labels too
            int xPos = chartX + (int)(data.size() - 1 - i) * chartW / (int)(data.size() - 1);
            std::string date = data[i].date.substr(5);
            fl_draw(date.c_str(), xPos - 15, chartY + chartH + 15);
        }
    }
    
    int handle(int event) override {
        return Fl_Widget::handle(event);
    }
}; 