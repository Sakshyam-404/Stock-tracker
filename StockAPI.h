#pragma once
#include <string>
#include "StockData.h"

class StockAPI {
public:
    static StockData fetchStock(const std::string& symbol);
};
