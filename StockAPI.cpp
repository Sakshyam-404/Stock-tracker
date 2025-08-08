#include "StockAPI.h"
#include <iostream>
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <sstream>
#include <regex>

#pragma comment(lib, "winhttp.lib")

// Simple JSON parser helper functions
std::string extractJsonValue(const std::string& json, const std::string& key) {
    std::string pattern = "\"" + key + "\"\\s*:\\s*\"([^\"]*)\"";
    std::regex regex(pattern);
    std::smatch match;
    if (std::regex_search(json, match, regex)) {
        return match[1].str();
    }
    return "";
}

float extractJsonFloat(const std::string& json, const std::string& key) {
    std::string pattern = "\"" + key + "\"\\s*:\\s*([+-]?\\d*\\.?\\d+)";
    std::regex regex(pattern);
    std::smatch match;
    if (std::regex_search(json, match, regex)) {
        try {
            return std::stof(match[1].str());
        } catch (...) {
            return 0.0f;
        }
    }
    return 0.0f;
}

// Helper function to make HTTP requests using WinHTTP
std::string makeHttpRequest(const std::string& url) {
    std::string result;
    
    // Parse URL
    std::string host, path;
    size_t protocolEnd = url.find("://");
    if (protocolEnd != std::string::npos) {
        size_t hostStart = protocolEnd + 3;
        size_t pathStart = url.find("/", hostStart);
        if (pathStart != std::string::npos) {
            host = url.substr(hostStart, pathStart - hostStart);
            path = url.substr(pathStart);
        } else {
            host = url.substr(hostStart);
            path = "/";
        }
    } else {
        return result;
    }
    
    // Initialize WinHTTP
    HINTERNET hSession = WinHttpOpen(L"Stock Tracker/1.0", 
                                   WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                   WINHTTP_NO_PROXY_NAME, 
                                   WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        return result;
    }
    
    // Connect to server
    HINTERNET hConnect = WinHttpConnect(hSession, 
                                      std::wstring(host.begin(), host.end()).c_str(),
                                      INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return result;
    }
    
    // Create request
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET",
                                          std::wstring(path.begin(), path.end()).c_str(),
                                          NULL, WINHTTP_NO_REFERER,
                                          WINHTTP_DEFAULT_ACCEPT_TYPES,
                                          WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return result;
    }
    
    // Send request
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return result;
    }
    
    // Receive response
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return result;
    }
    
    // Read response data
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    
    do {
        // Check for available data
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            break;
        }
        
        // Allocate space for the buffer
        pszOutBuffer = new char[dwSize + 1];
        if (!pszOutBuffer) {
            break;
        }
        
        // Read the data
        ZeroMemory(pszOutBuffer, dwSize + 1);
        if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
            delete[] pszOutBuffer;
            break;
        }
        
        // Append to result
        result.append(pszOutBuffer, dwDownloaded);
        delete[] pszOutBuffer;
        
    } while (dwSize > 0);
    
    // Clean up
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    
    return result;
}

StockData StockAPI::fetchStock(const std::string& symbol) {
    std::string url = "https://financialmodelingprep.com/api/v3/quote/" + symbol + "?apikey=Rjopaz0vhf8OcX4j6qvrO3pnj9MxhgfZ";
    
    std::string response = makeHttpRequest(url);
    
    if (response.empty()) {
        std::cerr << "Failed to fetch data for " << symbol << std::endl;
        return StockData(symbol);
    }
    
    // Print response to help debug
    std::cout << "API response: " << response << std::endl;
    
    // Check if response is valid JSON array
    if (response.find("[") == std::string::npos || response.find("]") == std::string::npos) {
        std::cerr << "Invalid JSON response for " << symbol << std::endl;
        return StockData(symbol);
    }
    
    // Extract stock data using regex
    float price = extractJsonFloat(response, "price");
    float change = extractJsonFloat(response, "change");
    float changePercent = extractJsonFloat(response, "changesPercentage");
    float high = extractJsonFloat(response, "dayHigh");
    float low = extractJsonFloat(response, "dayLow");
    
    return StockData(symbol, price, change, changePercent, high, low);
}

std::vector<CandlestickData> StockAPI::fetchHistoricalData(const std::string& symbol, int days) {
    std::vector<CandlestickData> result;
    
    // Use the historical daily data endpoint
    std::string url = "https://financialmodelingprep.com/api/v3/historical-price-full/" + 
                      symbol + "?apikey=Rjopaz0vhf8OcX4j6qvrO3pnj9MxhgfZ";
    
    std::string response = makeHttpRequest(url);
    
    if (response.empty()) {
        std::cerr << "Failed to fetch historical data for " << symbol << std::endl;
        return result;
    }
    
    // Check if response contains historical data
    if (response.find("\"historical\"") == std::string::npos) {
        std::cerr << "No historical data found for " << symbol << std::endl;
        return result;
    }
    
    // Extract historical data using regex
    std::regex dateRegex("\"date\"\\s*:\\s*\"([^\"]*)\"");
    std::regex openRegex("\"open\"\\s*:\\s*([+-]?\\d*\\.?\\d+)");
    std::regex highRegex("\"high\"\\s*:\\s*([+-]?\\d*\\.?\\d+)");
    std::regex lowRegex("\"low\"\\s*:\\s*([+-]?\\d*\\.?\\d+)");
    std::regex closeRegex("\"close\"\\s*:\\s*([+-]?\\d*\\.?\\d+)");
    std::regex volumeRegex("\"volume\"\\s*:\\s*([+-]?\\d*\\.?\\d+)");
    
    std::sregex_iterator dateIter(response.begin(), response.end(), dateRegex);
    std::sregex_iterator openIter(response.begin(), response.end(), openRegex);
    std::sregex_iterator highIter(response.begin(), response.end(), highRegex);
    std::sregex_iterator lowIter(response.begin(), response.end(), lowRegex);
    std::sregex_iterator closeIter(response.begin(), response.end(), closeRegex);
    std::sregex_iterator volumeIter(response.begin(), response.end(), volumeRegex);
    
    int count = 0;
    while (dateIter != std::sregex_iterator() && count < days) {
        try {
            std::string date = (*dateIter)[1].str();
            float open = std::stof((*openIter)[1].str());
            float high = std::stof((*highIter)[1].str());
            float low = std::stof((*lowIter)[1].str());
            float close = std::stof((*closeIter)[1].str());
            float volume = std::stof((*volumeIter)[1].str());
            
            result.emplace_back(date, open, high, low, close, volume);
            count++;
            
            ++dateIter;
            ++openIter;
            ++highIter;
            ++lowIter;
            ++closeIter;
            ++volumeIter;
        } catch (...) {
            break;
        }
    }
    
    return result;
}
