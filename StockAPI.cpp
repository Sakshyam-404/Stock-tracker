#include <nlohmann/json.hpp>
#include "StockAPI.h"
#include <cpr/cpr.h>
#include <iostream>

using json = nlohmann::json;

StockData StockAPI::fetchStock(const std::string& symbol) {
std::string url = "https://financialmodelingprep.com/api/v3/quote/" + symbol + "?apikey=Rjopaz0vhf8OcX4j6qvrO3pnj9MxhgfZ";

   

    cpr::Response r = cpr::Get(cpr::Url{ url });

    // Print response to help debug
    std::cout << "API response: " << r.text << std::endl;

    // Safely try to parse JSON
    json j;
    try {
        j = json::parse(r.text);
    }
    catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return StockData(symbol); // fallback empty
    }

    if (!j.is_array() || j.empty()) {
        std::cerr << "Unexpected JSON structure or empty response.\n";
        return StockData(symbol);
    }

    const auto& stock = j[0];

    float price = stock.contains("price") && stock["price"].is_number() ? stock["price"].get<float>() : 0.0f;
    float change = stock.contains("change") && stock["change"].is_number() ? stock["change"].get<float>() : 0.0f;
    float changePercent = stock.contains("changesPercentage") && stock["changesPercentage"].is_number() ? stock["changesPercentage"].get<float>() : 0.0f;
    float high = stock.contains("dayHigh") && stock["dayHigh"].is_number() ? stock["dayHigh"].get<float>() : 0.0f;
    float low = stock.contains("dayLow") && stock["dayLow"].is_number() ? stock["dayLow"].get<float>() : 0.0f;

    return StockData(symbol, price, change, changePercent, high, low);
}
