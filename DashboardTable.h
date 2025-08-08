#pragma once
#include <FL/Fl_Table_Row.H>
#include <vector>
#include "StockData.h"

class DashboardTable : public Fl_Table_Row {
private:
    std::vector<StockData>* portfolio;

protected:
    void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) override;

public:
    DashboardTable(int X, int Y, int W, int H, const char* L = 0);
    void setPortfolio(std::vector<StockData>* p);
    void refresh();
};
