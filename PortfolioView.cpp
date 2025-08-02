
#include "PortfolioView.h"

PortfolioView::PortfolioView(int x, int y, int w, int h) {
    group = new Fl_Group(x, y, w, h);
    output = new Fl_Multiline_Output(x + 10, y + 10, w - 20, h - 20);
    group->end();
}

Fl_Group* PortfolioView::getGroup() const {
    return group;
}

void PortfolioView::updateDisplay(const Portfolio& portfolio) {
    std::ostringstream oss;
    const auto& stocks = portfolio.getStocks();

    if (stocks.empty()) {
        oss << "Your portfolio is empty.\n";
    }
    else {
        oss << "Portfolio Stocks:\n\n";
        for (const auto& s : stocks) {
            oss << s.getSymbol() << " - $" << s.getPrice()
                << " (Change: " << s.getChangePercent() << "%)\n";
        }
    }

    output->value(oss.str().c_str());
}
