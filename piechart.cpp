//#include "PieChart.h"
//#include <FL/fl_draw.H>
//#include <cmath>
//
//PieChart::PieChart(int x, int y, int w, int h)
//    : Fl_Widget(x, y, w, h) {
//}
//
//void PieChart::setData(const std::vector<std::string>& lbls, const std::vector<int>& vals) {
//    labels = lbls;
//    values = vals;
//    redraw();
//}
//
//void PieChart::draw() {
//    if (values.empty()) return;
//
//    int total = 0;
//    for (int v : values) total += v;
//    if (total == 0) return;
//
//    int cx = x() + w() / 2;
//    int cy = y() + h() / 2;
//    int radius = std::min(w(), h()) / 2 - 10;
//    double start_angle = 0.0;
//
//    // Draw pie slices
//    for (size_t i = 0; i < values.size(); ++i) {
//        double angle = (double)values[i] / total * 360.0;
//        fl_color(static_cast<Fl_Color>(i * 32 + 1));  // +1 to avoid FL_BACKGROUND_COLOR
//        fl_pie(cx - radius, cy - radius, 2 * radius, 2 * radius, start_angle, start_angle + angle);
//
//        // Draw labels
//        double mid_angle = start_angle + angle / 2;
//        double rad = mid_angle * (M_PI / 180.0);
//        int tx = cx + (radius * 0.6) * cos(rad);
//        int ty = cy + (radius * 0.6) * sin(rad);
//        fl_color(FL_BLACK);
//        fl_draw(labels[i].c_str(), tx - 10, ty - 5);
//
//        start_angle += angle;
//    }
//
//    // Draw border
//    fl_color(FL_BLACK);
//    fl_arc(cx - radius, cy - radius, 2 * radius, 2 * radius, 0, 360);
//}