#include "Portfolio.h"

void Portfolio::addStock(const StockData& stock) {
    stocks.push_back(stock);
}

const std::vector<StockData>& Portfolio::getStocks() const {
    return stocks;
}