
#pragma once
#include <vector>
#include "StockData.h"

class Portfolio {
    std::vector<StockData> stocks;

public:
    void addStock(const StockData& stock);
    const std::vector<StockData>& getStocks() const;
};
