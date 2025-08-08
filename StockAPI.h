#pragma once
#include <string>
#include <vector>
#include "StockData.h"
#include "CandlestickData.h"

class StockAPI {
public:
    static StockData fetchStock(const std::string& symbol);
    static std::vector<CandlestickData> fetchHistoricalData(const std::string& symbol, int days = 7);
};
