#pragma once
#include <string>
#include <vector>
#include <map>

struct PortfolioEntry {
    std::string symbol;
    int shares;
    float waccPrice;  // Renamed from avgPrice to waccPrice
    float currentPrice;
    
    PortfolioEntry(const std::string& s, int sh, float price) 
        : symbol(s), shares(sh), waccPrice(price), currentPrice(0.0f) {}
    
    float getTotalValue() const { return shares * currentPrice; }
    float getTotalCost() const { return shares * waccPrice; }  // Using WACC price
    float getProfitLoss() const { return getTotalValue() - getTotalCost(); }
    float getProfitLossPercent() const { 
        if (getTotalCost() == 0) return 0.0f;
        return (getProfitLoss() / getTotalCost()) * 100.0f; 
    }
};

class Portfolio {
private:
    std::vector<PortfolioEntry> entries;
    float cashBalance = 10000.0f; // Initialize directly with default value
    const float INITIAL_BALANCE = 10000.0f; // $10,000 starting balance
    
public:
    Portfolio() = default; // Use default constructor since cashBalance is initialized inline
    
    // Getters
    float getCashBalance() const { return cashBalance; }
    const std::vector<PortfolioEntry>& getEntries() const { return entries; }
    float getTotalPortfolioValue() const;
    float getTotalProfitLoss() const;
    
    // Trading methods
    bool buyStock(const std::string& symbol, int shares, float price);
    bool sellStock(const std::string& symbol, int shares, float price);
    void updatePrices(const std::map<std::string, float>& prices);
    
    // Portfolio management
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    void clear();
    
    // Utility methods
    PortfolioEntry* findEntry(const std::string& symbol);
    const PortfolioEntry* findEntry(const std::string& symbol) const;
    bool hasStock(const std::string& symbol) const;
    int getShares(const std::string& symbol) const;
}; 