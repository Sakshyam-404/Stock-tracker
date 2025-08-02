//#include "PortfolioTable.h"
//#include <FL/fl_draw.H>
//#include <iomanip>
//#include <sstream>
//
//PortfolioTable::PortfolioTable(int X, int Y, int W, int H)
//    : Fl_Table_Row(X, Y, W, H), portfolio(nullptr) {
//    cols(7);
//    col_header(1);
//    col_resize(1);
//    col_width_all(W / 7);
//    end();
//}
//
//void PortfolioTable::setPortfolio(const Portfolio* p) {
//    portfolio = p;
//    rows(portfolio ? portfolio->getStocks().size() : 0);
//    redraw();
//}
//
//void PortfolioTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) {
//    if (!portfolio || R >= portfolio->getStocks().size()) return;
//
//    fl_push_clip(X, Y, W, H);
//
//    // Column headers
//    if (context == CONTEXT_COL_HEADER) {
//        static const char* headers[] = { "Symbol", "Price", "Change", "Chg%", "High", "Low", "Shares" };
//        fl_draw(headers[C], X + 2, Y + H - 4);
//        fl_pop_clip();
//        return;
//    }
//
//    // Cell content
//    const StockData& s = portfolio->getStocks()[R];
//    std::ostringstream oss;
//    oss << std::fixed << std::setprecision(2);
//
//    switch (C) {
//    case 0: oss << s.getSymbol(); break;
//    case 1: oss << "$" << s.getPrice(); break;
//    case 2: oss << (s.getChange() >= 0 ? "+" : "") << s.getChange(); break;
//    case 3: oss << s.getChangePercent() << "%"; break;
//    case 4: oss << "$" << s.getHigh(); break;
//    case 5: oss << "$" << s.getLow(); break;
//    case 6: oss << s.getShares(); break;
//    }
//
//    fl_color(FL_WHITE); fl_rectf(X, Y, W, H);
//    fl_color(FL_BLACK);
//    fl_draw(oss.str().c_str(), X + (C > 0 ? W - fl_width(oss.str().c_str()) - 5 : 5), Y + H - 4);
//    fl_pop_clip();
//}