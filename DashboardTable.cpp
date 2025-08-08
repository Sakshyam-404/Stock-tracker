#include "DashboardTable.h"
#include <FL/fl_draw.H>
#include <sstream>

// Remove all implementation, as DashboardTable is not used anymore
DashboardTable::DashboardTable(int X, int Y, int W, int H, const char* L)
    : Fl_Table_Row(X, Y, W, H, L), portfolio(nullptr)
{
}
void DashboardTable::setPortfolio(std::vector<StockData>* p) {}
void DashboardTable::refresh() {}
void DashboardTable::draw_cell(TableContext, int, int, int, int, int, int) {}
