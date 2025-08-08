
#include "Portfolio.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

float Portfolio::getTotalPortfolioValue() const {
    float total = cashBalance;
    for (const auto& entry : entries) {
        total += entry.getTotalValue();
    }
    return total;
}

float Portfolio::getTotalProfitLoss() const {
    float total = 0.0f;
    for (const auto& entry : entries) {
        total += entry.getProfitLoss();
    }
    return total;
}

bool Portfolio::buyStock(const std::string& symbol, int shares, float price) {
    float totalCost = shares * price;
    
    if (totalCost > cashBalance) {
        return false; // Insufficient funds
    }
    
    auto* entry = findEntry(symbol);
    if (entry) {
        // Update existing position with WACC calculation
        int newShares = entry->shares + shares;
        float newWaccPrice = ((entry->shares * entry->waccPrice) + (shares * price)) / newShares;
        entry->shares = newShares;
        entry->waccPrice = newWaccPrice;  // WACC price
    } else {
        // Add new position
        entries.emplace_back(symbol, shares, price);
    }
    
    cashBalance -= totalCost;
    return true;
}

bool Portfolio::sellStock(const std::string& symbol, int shares, float price) {
    auto* entry = findEntry(symbol);
    if (!entry || entry->shares < shares) {
        return false; // Insufficient shares
    }
    
    float totalValue = shares * price;
    entry->shares -= shares;
    
    if (entry->shares == 0) {
        // Remove entry if no shares left
        entries.erase(std::remove_if(entries.begin(), entries.end(),
            [&symbol](const PortfolioEntry& e) { return e.symbol == symbol; }), entries.end());
    }
    
    cashBalance += totalValue;
    return true;
}

void Portfolio::updatePrices(const std::map<std::string, float>& prices) {
    for (auto& entry : entries) {
        auto it = prices.find(entry.symbol);
        if (it != prices.end()) {
            entry.currentPrice = it->second;
        }
    }
}

PortfolioEntry* Portfolio::findEntry(const std::string& symbol) {
    for (auto& entry : entries) {
        if (entry.symbol == symbol) {
            return &entry;
        }
    }
    return nullptr;
}

const PortfolioEntry* Portfolio::findEntry(const std::string& symbol) const {
    for (const auto& entry : entries) {
        if (entry.symbol == symbol) {
            return &entry;
        }
    }
    return nullptr;
}

bool Portfolio::hasStock(const std::string& symbol) const {
    return findEntry(symbol) != nullptr;
}

int Portfolio::getShares(const std::string& symbol) const {
    const auto* entry = findEntry(symbol);
    return entry ? entry->shares : 0;
}

void Portfolio::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << cashBalance << "\n";
        for (const auto& entry : entries) {
            file << entry.symbol << "," << entry.shares << "," << entry.waccPrice << "\n";
        }
        file.close();
    }
}

void Portfolio::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        entries.clear();
        std::string line;
        
        // Read cash balance
        if (std::getline(file, line)) {
            try {
                cashBalance = std::stof(line);
            } catch (...) {
                cashBalance = INITIAL_BALANCE;
            }
        }
        
        // Read portfolio entries
        while (std::getline(file, line)) {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                std::string symbol = line.substr(0, pos1);
                int shares = std::stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
                float waccPrice = std::stof(line.substr(pos2 + 1));
                entries.emplace_back(symbol, shares, waccPrice);
            }
        }
        file.close();
    }
} 

void Portfolio::clear() {
    entries.clear();
    cashBalance = INITIAL_BALANCE;
} 