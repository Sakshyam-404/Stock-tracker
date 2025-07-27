
#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Multiline_Output.H>
#include "Portfolio.h"
#include <sstream>

class PortfolioView {
    Fl_Group* group;
    Fl_Multiline_Output* output;

public:
    PortfolioView(int x, int y, int w, int h);
    Fl_Group* getGroup() const;
    void updateDisplay(const Portfolio& portfolio);
};
