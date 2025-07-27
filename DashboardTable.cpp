#include "DashboardTable.h"
#include <FL/fl_draw.H>
#include <sstream>

DashboardTable::DashboardTable(int X, int Y, int W, int H, const char* L)
    : Fl_Table_Row(X, Y, W, H, L), portfolio(nullptr)
{
    col_header(1);
    col_resize(1);
    row_header(0);
    cols(6); // symbol, price, change, %change, high, low
    col_width_all(120);
    end();
}

void DashboardTable::setPortfolio(std::vector<StockData>* p) {
    portfolio = p;
    rows(p ? p->size() : 0);
    redraw();
}

void DashboardTable::refresh() {
    if (portfolio) rows(portfolio->size());
    redraw();
}

void DashboardTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) {
    if (!portfolio || R >= portfolio->size()) return;

    fl_push_clip(X, Y, W, H);
    fl_draw_box(FL_FLAT_BOX, X, Y, W, H, row_selected(R) ? FL_LIGHT2 : FL_WHITE);
    fl_color(FL_BLACK);
    fl_font(FL_HELVETICA, 14);

    const StockData& s = (*portfolio)[R];
    std::ostringstream oss;

    switch (C) {
    case 0: oss << s.getSymbol(); break;
    case 1: oss << s.getPrice(); break;
    case 2: oss << s.getChange(); break;
    case 3: oss << s.getChangePercent() << "%"; break;
    case 4: oss << s.getHigh(); break;
    case 5: oss << s.getLow(); break;
    default: break;
    }

    fl_draw(oss.str().c_str(), X + 4, Y + H - 6);
    fl_pop_clip();
}
