#include "AppMainWindow.h"
#include "StockAPI.h"
#include <sstream>
#include <iomanip>
#include "DashboardTable.h"
#include <FL/Fl_Chart.H>
#include "LineChart.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include "Portfolio.h"
#include "AuthWindow.h"
AppMainWindow::AppMainWindow(int w, int h, const char *title)
    : Fl_Window(w, h, title)
{
    this->resizable(this);

    int sidebarWidth = 200;
    int padding = 10;

    // Sidebar
    sidebarGroup = new Fl_Group(0, 0, sidebarWidth, h);
    
    // Welcome message in sidebar
    welcomeLabel = new Fl_Box(10, 10, 180, 30, "Hi, Guest!");
    welcomeLabel->labelsize(14);
    welcomeLabel->labelfont(FL_BOLD);
    welcomeLabel->labelcolor(FL_BLUE);
    welcomeLabel->align(FL_ALIGN_CENTER);
    
    tradeBtn = new Fl_Button(10, 50, 180, 40, "Trade");
    wishlistBtn = new Fl_Button(10, 100, 180, 40, "Wishlist");
    portfolioBtn = new Fl_Button(10, 150, 180, 40, "Portfolio");
    logoutBtn = new Fl_Button(10, 200, 180, 40, "Logout");
    deleteAccountBtn = new Fl_Button(10, 250, 180, 40, "Delete Account");
    
    tradeBtn->callback(onTradeBtnClicked, this);
    wishlistBtn->callback(onWishlistBtnClicked, this);
    portfolioBtn->callback(onPortfolioBtnClicked, this);
    logoutBtn->callback(onLogoutClicked, this);
    deleteAccountBtn->callback(onDeleteAccountClicked, this);
    
    // Make delete button red to indicate danger
    deleteAccountBtn->color(FL_RED);
    deleteAccountBtn->labelcolor(FL_WHITE);
    
    sidebarGroup->end();

    // Trade page
    tradeGroup = new Fl_Group(sidebarWidth + 30, 0, w - sidebarWidth - 30, h);
    
    // Search section
    stockSymbolInput = new Fl_Input(sidebarWidth + padding + 30, 30, 200, 30, "Symbol:");
    stockSymbolInput->when(FL_WHEN_ENTER_KEY_ALWAYS);
    stockSymbolInput->callback(onSearchClicked, this);
    searchButton = new Fl_Button(sidebarWidth + 260 + 30, 30, 80, 30, "Search");
    searchButton->callback(onSearchClicked, this);
    
    // Add button
    addButton = new Fl_Button(sidebarWidth + 350 + 30, 30, 80, 30, "Add");
    addButton->callback(onAddClicked, this);
    
    // Buy/Sell section with colors
    buyButton = new Fl_Button(sidebarWidth + 440 + 30, 30, 80, 30, "Buy");
    sellButton = new Fl_Button(sidebarWidth + 530 + 30, 30, 80, 30, "Sell");
    buyButton->callback(onBuyClicked, this);
    sellButton->callback(onSellClicked, this);
    buyButton->deactivate();
    sellButton->deactivate();
    
    // Add colors
    buyButton->color(FL_BLUE);
    buyButton->labelcolor(FL_WHITE);
    sellButton->color(FL_RED);
    sellButton->labelcolor(FL_WHITE);
    
    // Quantity input
    quantityInput = new Fl_Input(sidebarWidth + padding + 30, 70, 100, 30, "Quantity:");

    stockTextBuffer = new Fl_Text_Buffer();
    stockOutputDisplay = new Fl_Text_Display(sidebarWidth + padding + 30, 110, 400, 150);
    stockOutputDisplay->buffer(stockTextBuffer);

    // Add line chart
    lineChart = new LineChart(sidebarWidth + padding + 30, 280, 600, 300, "Price Chart");

    tradeGroup->end();

    // Wishlist
    wishlistGroup = new Fl_Group(sidebarWidth, 0, w - sidebarWidth, h);
    wishlistGroup->end();
    wishlistGroup->hide();
    
    wishlistOutput = new Fl_Multiline_Output(sidebarWidth + 10, 30, w - sidebarWidth - 20, h - 100);
    wishlistOutput->readonly(1);
    wishlistOutput->callback(onWishlistOutputClicked, this);
    wishlistGroup->add(wishlistOutput);
    
    removeStockBtn = new Fl_Button(w - 120, h - 50, 100, 30, "Remove Selected");
    removeStockBtn->callback(onRemoveStockClicked, this);
    removeStockBtn->deactivate();
    wishlistGroup->add(removeStockBtn);
    
    updateWishlistTable();
    
    // Portfolio page
    portfolioGroup = new Fl_Group(sidebarWidth, 0, w - sidebarWidth, h);
    portfolioGroup->end();
    portfolioGroup->hide();
    
    portfolioOutput = new Fl_Multiline_Output(sidebarWidth + 10, 30, w - sidebarWidth - 20, h - 60);
    portfolioOutput->readonly(1);
    portfolioOutput->textfont(FL_COURIER);
    portfolioOutput->textsize(16);
    portfolioGroup->add(portfolioOutput);
    
    // Load saved data
    portfolio.loadFromFile(getPortfolioFilename());
    loadWishlistFromFile();
    updatePortfolioDisplay();

    end();
    switchTo(tradeGroup);
}

AppMainWindow::~AppMainWindow() {
    portfolio.saveToFile(getPortfolioFilename());
    saveWishlistToFile();
}

void AppMainWindow::switchTo(Fl_Group *target)
{
    tradeGroup->hide();
    wishlistGroup->hide();
    portfolioGroup->hide();
    target->show();
    redraw();
}

void AppMainWindow::updateWishlistTable()
{
    wishlistOutput->textfont(FL_COURIER);
    wishlistOutput->textsize(16);
    
    std::ostringstream oss;
    oss.precision(2);
    oss << std::fixed;
    
    // Header row
    oss << std::setw(12) << "Symbol" << " | ";
    oss << std::setw(15) << "Price" << " | ";
    oss << std::setw(15) << "Change" << " | ";
    oss << std::setw(12) << "Change%" << " | ";
    oss << std::setw(15) << "High" << " | ";
    oss << std::setw(15) << "Low" << "\n";
    
    // Separator line
    oss << std::string(12, '-') << "-+-" << std::string(15, '-') << "-+-" 
        << std::string(15, '-') << "-+-" << std::string(12, '-') << "-+-" 
        << std::string(15, '-') << "-+-" << std::string(15, '-') << "\n";
    
    // Data rows with multiple selection indicators
    for (size_t i = 0; i < wishlist.size(); i++) {
        const auto& stock = wishlist[i];
        
        // Fetch current stock data from API
        StockData currentStock = StockAPI::fetchStock(stock.getSymbol());
        
        // Multiple selection indicator
        if (selectedStockIndices.find(i) != selectedStockIndices.end()) {
            oss << " [X] "; // Selected indicator
        } else {
            oss << " [ ] "; // Unselected indicator
        }
        
        oss << std::setw(8) << stock.getSymbol() << " | ";
        oss << std::setw(15) << currentStock.getPrice() << " | ";
        
        std::string changeStr = (currentStock.getChange() >= 0 ? "+" : "") + std::to_string(currentStock.getChange());
        oss << std::setw(15) << changeStr << " | ";
        
        std::string changePercentStr = (currentStock.getChangePercent() >= 0 ? "+" : "") + std::to_string(currentStock.getChangePercent()) + "%";
        oss << std::setw(12) << changePercentStr << " | ";
        
        oss << std::setw(15) << currentStock.getHigh() << " | ";
        oss << std::setw(15) << currentStock.getLow() << "\n";
    }
    
    if (wishlist.empty()) {
        oss << "\nNo stocks in wishlist yet.\n";
        oss << "Go to Trade tab to search and add stocks to watch.\n";
        oss << "\nClick on [ ] to select stocks, then click 'Remove Selected'.\n";
    } else {
        oss << "\nClick on [ ] to select multiple stocks, then click 'Remove Selected'.\n";
        if (!selectedStockIndices.empty()) {
            oss << "Selected: " << selectedStockIndices.size() << " stock(s)\n";
        }
    }
    
    wishlistOutput->value(oss.str().c_str());
    
    // Update remove button state
    if (!selectedStockIndices.empty()) {
        removeStockBtn->activate();
    } else {
        removeStockBtn->deactivate();
    }
}

void AppMainWindow::saveWishlistToFile() {
    std::ofstream file(getWishlistFilename());
    if (file.is_open()) {
        for (const auto& stock : wishlist) {
            file << stock.getSymbol() << "\n";
        }
        file.close();
    }
}

void AppMainWindow::loadWishlistFromFile() {
    std::ifstream file(getWishlistFilename());
    if (!file.is_open()) {
        // File missing, handle gracefully
        wishlist.clear();
        updateWishlistTable();
        return;
    }
    wishlist.clear();
    std::string symbol;
    while (std::getline(file, symbol)) {
        if (!symbol.empty()) {
            StockData stock = StockAPI::fetchStock(symbol);
            if (!stock.getSymbol().empty() && stock.getPrice() != 0.0f) {
                wishlist.push_back(stock);
            }
        }
    }
    file.close();
    updateWishlistTable();
}

void AppMainWindow::removeSelectedStocks() {
    if (selectedStockIndices.empty()) {
        return;
    }
    
    // Remove stocks in reverse order to maintain indices
    std::vector<int> indicesToRemove(selectedStockIndices.begin(), selectedStockIndices.end());
    std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<int>());
    
    for (int index : indicesToRemove) {
        if (index >= 0 && index < (int)wishlist.size()) {
            wishlist.erase(wishlist.begin() + index);
        }
    }
    
    selectedStockIndices.clear();
    updateWishlistTable();
    saveWishlistToFile();
}

void AppMainWindow::toggleStockSelection(int index) {
    if (index >= 0 && index < (int)wishlist.size()) {
        if (selectedStockIndices.find(index) != selectedStockIndices.end()) {
            selectedStockIndices.erase(index);
        } else {
            selectedStockIndices.insert(index);
        }
        updateWishlistTable();
    }
}

void AppMainWindow::fetchStockAndDisplay()
{
    std::string symbol = stockSymbolInput->value();
    if (symbol.empty())
        return;

    StockData stock = StockAPI::fetchStock(symbol);
    if (stock.getSymbol().empty() || stock.getPrice() == 0.0f) {
        stockTextBuffer->text("Invalid stock symbol or not found.\n");
        buyButton->deactivate();
        sellButton->deactivate();
        addButton->deactivate();
        return;
    }

    std::ostringstream oss;
    oss.precision(2);
    oss << std::fixed;

    oss << "=== STOCK INFO ===\n\n";
    oss << "Symbol         : " << stock.getSymbol() << "\n";
    oss << "Price          : $" << stock.getPrice() << "\n";
    oss << "Change         : " << (stock.getChange() >= 0 ? "+" : "") << stock.getChange() << "\n";
    oss << "Change Percent : " << (stock.getChangePercent() >= 0 ? "+" : "") << stock.getChangePercent() << "%\n";
    oss << "Day High       : $" << stock.getHigh() << "\n";
    oss << "Day Low        : $" << stock.getLow() << "\n";

    std::string text = oss.str();
    stockTextBuffer->text(text.c_str());

    stockOutputDisplay->textfont(FL_HELVETICA_BOLD);
    stockOutputDisplay->textsize(16);
    
    updateCandlestickChart(symbol);
    
    // Enable buttons
    buyButton->activate();
    sellButton->activate();
    addButton->activate();
}

void AppMainWindow::executeTrade(bool isBuy) {
    std::string symbol = stockSymbolInput->value();
    std::string quantityStr = quantityInput->value();
    
    if (symbol.empty() || quantityStr.empty()) {
        stockTextBuffer->text("Please enter symbol and quantity.\n");
        return;
    }
    
    try {
        int quantity = std::stoi(quantityStr);
        if (quantity <= 0) {
            stockTextBuffer->text("Quantity must be positive.\n");
            return;
        }
        
        StockData stock = StockAPI::fetchStock(symbol);
        if (stock.getSymbol().empty()) {
            stockTextBuffer->text("Invalid stock symbol.\n");
            return;
        }
        
        float price = stock.getPrice();
        float total = quantity * price;
        
        bool success = false;
        if (isBuy) {
            success = portfolio.buyStock(symbol, quantity, price);
            if (success) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2);
                oss << "Bought " << quantity << " shares of " << symbol 
                    << " at $" << price << " each. Total: $" << total << "\n";
                stockTextBuffer->text(oss.str().c_str());
            } else {
                stockTextBuffer->text("Insufficient funds for this purchase.\n");
                return;
            }
        } else {
            success = portfolio.sellStock(symbol, quantity, price);
            if (success) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2);
                oss << "Sold " << quantity << " shares of " << symbol 
                    << " at $" << price << " each. Total: $" << total << "\n";
                stockTextBuffer->text(oss.str().c_str());
            } else {
                stockTextBuffer->text("Insufficient shares to sell.\n");
                return;
            }
        }
        
        portfolio.saveToFile(getPortfolioFilename());
        quantityInput->value("");
        updatePortfolioDisplay();
        
    } catch (...) {
        stockTextBuffer->text("Invalid quantity format.\n");
    }
}

void AppMainWindow::updatePortfolioDisplay() {
    // Update prices for all portfolio entries from API
    std::map<std::string, float> prices;
    for (const auto& entry : portfolio.getEntries()) {
        StockData stock = StockAPI::fetchStock(entry.symbol);
        if (!stock.getSymbol().empty()) {
            prices[entry.symbol] = stock.getPrice();
        }
    }
    portfolio.updatePrices(prices);
    
    // Display portfolio summary
    std::ostringstream oss;
    oss.precision(2);
    oss << std::fixed;
    
    // Header row
    oss << std::setw(12) << "Symbol" << " | ";
    oss << std::setw(10) << "Shares" << " | ";
    oss << std::setw(15) << "WACC Price" << " | ";
    oss << std::setw(15) << "Current Price" << " | ";
    oss << std::setw(15) << "Total Value" << " | ";
    oss << std::setw(12) << "P&L" << " | ";
    oss << std::setw(10) << "P&L%" << "\n";
    
    // Separator line
    oss << std::string(12, '-') << "-+-" << std::string(10, '-') << "-+-" 
        << std::string(15, '-') << "-+-" << std::string(15, '-') << "-+-" 
        << std::string(15, '-') << "-+-" << std::string(12, '-') << "-+-" 
        << std::string(10, '-') << "\n";
    
    // Data rows
    for (const auto& entry : portfolio.getEntries()) {
        oss << std::setw(12) << entry.symbol << " | ";
        oss << std::setw(10) << entry.shares << " | ";
        oss << std::setw(15) << entry.waccPrice << " | ";
        oss << std::setw(15) << entry.currentPrice << " | ";
        oss << std::setw(15) << entry.getTotalValue() << " | ";
        
        std::string pnlStr = (entry.getProfitLoss() >= 0 ? "+" : "") + std::to_string(entry.getProfitLoss());
        oss << std::setw(12) << pnlStr << " | ";
        
        std::string pnlPercentStr = (entry.getProfitLossPercent() >= 0 ? "+" : "") + std::to_string(entry.getProfitLossPercent()) + "%";
        oss << std::setw(10) << pnlPercentStr << "\n";
    }
    
    if (portfolio.getEntries().empty()) {
        oss << "\nNo holdings yet.\n";
        oss << "Go to Trade tab to buy stocks.\n";
    }
    
    // Portfolio summary at bottom
    oss << "\n=== PORTFOLIO SUMMARY ===\n";
    oss << "Available Cash: $" << portfolio.getCashBalance() << "\n";
    
    int totalShares = 0;
    for (const auto& entry : portfolio.getEntries()) {
        totalShares += entry.shares;
    }
    oss << "Total Shares Owned: " << totalShares << "\n";
    oss << "Total Portfolio Value: $" << portfolio.getTotalPortfolioValue() << "\n";
    oss << "Total P&L: $" << portfolio.getTotalProfitLoss() << "\n";
    
    portfolioOutput->value(oss.str().c_str());
}

void AppMainWindow::updateCandlestickChart(const std::string& symbol) {
    lineChart->clear();
    
    std::vector<CandlestickData> historicalData = StockAPI::fetchHistoricalData(symbol, 7);
    
    if (historicalData.empty()) {
        return;
    }
    
    lineChart->setData(historicalData);
}

void AppMainWindow::savePortfolioToFile() {
    portfolio.saveToFile("portfolio.txt");
}

void AppMainWindow::loadPortfolioFromFile() {
    portfolio.loadFromFile("portfolio.txt");
}

// Update setCurrentUser to load user-specific data
void AppMainWindow::setCurrentUser(const std::string& user) { 
    currentUser = user; 
    std::string welcomeMsg = "Hi, " + user + "!";
    welcomeLabel->copy_label(welcomeMsg.c_str());
    
    // Load user-specific data
    portfolio.loadFromFile(getPortfolioFilename());
    loadWishlistFromFile();
    updatePortfolioDisplay();
}

// Callbacks
void AppMainWindow::onSearchClicked(Fl_Widget*, void* userdata) {
    static_cast<AppMainWindow*>(userdata)->fetchStockAndDisplay();
}

void AppMainWindow::onAddClicked(Fl_Widget*, void* userdata) {
    auto* app = static_cast<AppMainWindow*>(userdata);
    std::string symbol = app->stockSymbolInput->value();
    if (symbol.empty())
        return;

    StockData stock = StockAPI::fetchStock(symbol);
    if (stock.getSymbol().empty() || stock.getPrice() == 0.0f) {
        app->stockTextBuffer->text("Invalid stock symbol or not found.\n");
        return;
    }
    
    // Check if stock already exists in wishlist
    for (const auto& wishlistStock : app->wishlist) {
        if (wishlistStock.getSymbol() == stock.getSymbol()) {
            app->stockTextBuffer->text("Stock already in wishlist.\n");
            return;
        }
    }
    
    // Add to wishlist (not portfolio)
    app->wishlist.push_back(stock);
    app->updateWishlistTable();
    app->saveWishlistToFile();
    app->stockTextBuffer->text("Stock added to wishlist.\n");
}

void AppMainWindow::onBuyClicked(Fl_Widget*, void* userdata) {
    static_cast<AppMainWindow*>(userdata)->executeTrade(true);
}

void AppMainWindow::onSellClicked(Fl_Widget*, void* userdata) {
    static_cast<AppMainWindow*>(userdata)->executeTrade(false);
}

void AppMainWindow::onTradeBtnClicked(Fl_Widget*, void* userdata) {
    static_cast<AppMainWindow*>(userdata)->switchTo(static_cast<AppMainWindow*>(userdata)->tradeGroup);
}

void AppMainWindow::onWishlistBtnClicked(Fl_Widget*, void* userdata) {
    static_cast<AppMainWindow*>(userdata)->switchTo(static_cast<AppMainWindow*>(userdata)->wishlistGroup);
}

void AppMainWindow::onPortfolioBtnClicked(Fl_Widget*, void* userdata) {
    auto* app = static_cast<AppMainWindow*>(userdata);
    app->updatePortfolioDisplay();
    app->switchTo(app->portfolioGroup);
}

void AppMainWindow::onRemoveStockClicked(Fl_Widget*, void* userdata) {
    static_cast<AppMainWindow*>(userdata)->removeSelectedStocks();
}

void AppMainWindow::onWishlistOutputClicked(Fl_Widget*, void* userdata) {
    auto* app = static_cast<AppMainWindow*>(userdata);
    
    // Get click position relative to the wishlist output widget
    int y = Fl::event_y() - app->wishlistOutput->y();
    int x = Fl::event_x() - app->wishlistOutput->x();
    
    // Only process clicks within the widget bounds
    if (x < 0 || x > app->wishlistOutput->w() || y < 0 || y > app->wishlistOutput->h()) {
        return;
    }
    
    // Calculate row based on text position
    int headerLines = 2; // Header + separator line
    int rowHeight = 20; // More accurate row height
    int startY = headerLines * rowHeight;
    
    if (y < startY) {
        // Clicked on header, clear all selections
        app->selectedStockIndices.clear();
        app->updateWishlistTable();
        return;
    }
    
    int row = (y - startY) / rowHeight;
    
    // Check if click is within valid wishlist range
    if (row >= 0 && row < (int)app->wishlist.size()) {
        // Check if click is in the selection area (first few characters)
        if (x < 50) { // Selection indicator area
            app->toggleStockSelection(row);
        }
    } else {
        // Clicked outside valid rows, clear all selections
        app->selectedStockIndices.clear();
        app->updateWishlistTable();
    }
}

void AppMainWindow::deleteUserAccount() {
    // Create a modern, clean password dialog
    Fl_Window* dialog = new Fl_Window(600, 400, "Confirm Account Deletion");
    
    // Set background to light gray
    dialog->color(FL_LIGHT2);
    
    // Add a prominent title
    Fl_Box* titleBox = new Fl_Box(20, 30, 560, 50, "*** CONFIRM ACCOUNT DELETION ***");
    titleBox->labelsize(20);
    titleBox->labelfont(FL_HELVETICA_BOLD);
    titleBox->labelcolor(FL_DARK_RED);
    titleBox->align(FL_ALIGN_CENTER);
    dialog->add(titleBox);
    
    // Add warning message
    Fl_Box* warningBox = new Fl_Box(20, 90, 560, 60, 
        "This action cannot be undone!\n"
        "All your data will be permanently deleted.");
    warningBox->labelsize(14);
    warningBox->labelfont(FL_HELVETICA);
    warningBox->labelcolor(FL_DARK_RED);
    warningBox->align(FL_ALIGN_CENTER | FL_ALIGN_WRAP);
    dialog->add(warningBox);
    
    // Add separator line
    Fl_Box* separator = new Fl_Box(20, 160, 560, 2, "");
    separator->color(FL_DARK_RED);
    dialog->add(separator);
    
    // Add password label
    Fl_Box* label = new Fl_Box(20, 180, 560, 30, "Enter your password to confirm:");
    label->labelsize(16);
    label->labelfont(FL_HELVETICA_BOLD);
    label->labelcolor(FL_BLACK);
    label->align(FL_ALIGN_CENTER);
    dialog->add(label);
    
    // Password input field with better styling
    Fl_Input* passwordInput = new Fl_Input(100, 220, 400, 40);
    passwordInput->type(FL_SECRET_INPUT);
    passwordInput->textsize(16);
    passwordInput->textfont(FL_HELVETICA);
    dialog->add(passwordInput);
    
    // Error message label (initially hidden)
    Fl_Box* errorLabel = new Fl_Box(20, 270, 560, 30, "");
    errorLabel->labelsize(14);
    errorLabel->labelfont(FL_HELVETICA);
    errorLabel->labelcolor(FL_RED);
    errorLabel->align(FL_ALIGN_CENTER);
    errorLabel->hide();
    dialog->add(errorLabel);
    
    // Buttons with better styling
    Fl_Button* confirmBtn = new Fl_Button(200, 320, 120, 40, "DELETE ACCOUNT");
    confirmBtn->color(FL_RED);
    confirmBtn->labelcolor(FL_WHITE);
    confirmBtn->labelsize(14);
    confirmBtn->labelfont(FL_HELVETICA_BOLD);
    
    Fl_Button* cancelBtn = new Fl_Button(340, 320, 120, 40, "CANCEL");
    cancelBtn->color(FL_GRAY);
    cancelBtn->labelcolor(FL_WHITE);
    cancelBtn->labelsize(14);
    cancelBtn->labelfont(FL_HELVETICA_BOLD);
    
    dialog->add(confirmBtn);
    dialog->add(cancelBtn);
    
    // Store the dialog pointer and error label for callbacks
    dialog->user_data(this);
    confirmBtn->user_data(errorLabel);
    
    // Set up callbacks
    confirmBtn->callback(onConfirmDeleteClicked, dialog);
    cancelBtn->callback([](Fl_Widget*, void* d) { 
        static_cast<Fl_Window*>(d)->hide(); 
    }, dialog);
    
    dialog->show();
    
    // Wait for user input
    while (dialog->shown()) {
        Fl::wait();
    }
    
    delete dialog;
}

void AppMainWindow::showDeletionMessage() {
    // Create a modern, clean dialog box
    Fl_Window* deletionWindow = new Fl_Window(700, 300, "Account Deletion Complete");
    
    // Make the window modal
    deletionWindow->set_modal();
    
    // Set background to light gray
    deletionWindow->color(FL_LIGHT2);
    
    // Add a simple, clean title
    Fl_Box* titleBox = new Fl_Box(20, 20, 660, 40, "Account Deletion Complete");
    titleBox->labelsize(20);
    titleBox->labelfont(FL_HELVETICA_BOLD);
    titleBox->labelcolor(FL_DARK_RED);
    titleBox->align(FL_ALIGN_CENTER);
    deletionWindow->add(titleBox);
    
    // Add a separator line
    Fl_Box* separator = new Fl_Box(20, 70, 660, 2, "");
    separator->color(FL_DARK_RED);
    deletionWindow->add(separator);
    
    // Add the main message with simple, clear text
    Fl_Box* messageBox = new Fl_Box(40, 90, 620, 120, 
        "Your account has been successfully deleted.\n\n"
        "All your data has been permanently removed.\n"
        "The application will now close.");
    
    messageBox->labelsize(16);
    messageBox->labelfont(FL_HELVETICA);
    messageBox->labelcolor(FL_BLACK);
    messageBox->align(FL_ALIGN_CENTER | FL_ALIGN_WRAP);
    deletionWindow->add(messageBox);
    
    // Add a simple OK button
    Fl_Button* okBtn = new Fl_Button(300, 230, 100, 35, "OK");
    okBtn->color(FL_DARK_RED);
    okBtn->labelcolor(FL_WHITE);
    okBtn->labelsize(14);
    okBtn->labelfont(FL_HELVETICA_BOLD);
    okBtn->callback([](Fl_Widget*, void* w) { 
        static_cast<Fl_Window*>(w)->hide(); 
    }, deletionWindow);
    
    deletionWindow->add(okBtn);
    deletionWindow->show();
    
    // Wait for user to acknowledge
    while (deletionWindow->shown()) {
        Fl::wait();
    }
    
    delete deletionWindow;
}

void AppMainWindow::onConfirmDeleteClicked(Fl_Widget*, void* dialog) {
    auto* d = static_cast<Fl_Window*>(dialog);
    auto* app = static_cast<AppMainWindow*>(d->user_data());
    
    // Get password from the input field - FIXED INDEX
    Fl_Input* passwordInput = static_cast<Fl_Input*>(d->child(4)); // Password input (corrected index)
    Fl_Box* errorLabel = static_cast<Fl_Box*>(d->child(5)); // Error label (corrected index)
    
    std::string password = passwordInput->value();
    
    if (password.empty()) {
        errorLabel->copy_label("Please enter your password.");
        errorLabel->show();
        d->redraw();
        return;
    }
    
    // Validate password
    if (!app->validateUserPassword(app->currentUser, password)) {
        errorLabel->copy_label("Incorrect password. Please try again.");
        errorLabel->show();
        d->redraw();
        return;
    }
    
    // Password is correct - proceed with deletion
    d->hide();
    
    // Remove user from users.txt
    std::ifstream inFile("users.txt");
    std::ofstream outFile("temp_users.txt");
    std::string line;
    bool userFound = false;
    
    if (inFile.is_open() && outFile.is_open()) {
        while (std::getline(inFile, line)) {
            size_t pos = line.find(':', 0);
            if (pos != std::string::npos) {
                std::string storedUsername = line.substr(0, pos);
                if (storedUsername != app->currentUser) {
                    outFile << line << "\n";
                } else {
                    userFound = true;
                }
            }
        }
        inFile.close();
        outFile.close();
        
        // Replace original file
        if (userFound) {
            std::remove("users.txt");
            if (std::rename("temp_users.txt", "users.txt") != 0) {
                // Handle rename error
                std::remove("temp_users.txt"); // Clean up temp file
                return;
            }
        }
    }
    
    // Delete user-specific files
    std::string portfolioFile = app->getPortfolioFilename();
    std::string wishlistFile = app->getWishlistFilename();
    
    std::remove(portfolioFile.c_str());
    std::remove(wishlistFile.c_str());
    
    // Clear current data
    app->portfolio.clear();
    app->wishlist.clear();
    app->selectedStockIndices.clear();
    
    // Update displays
    app->updatePortfolioDisplay();
    app->updateWishlistTable();
    
    // Show deletion message
    app->showDeletionMessage();
    
    // Hide main window and terminate gracefully
    app->hide();
    
    // Use a different approach - set a flag to terminate after the message
    Fl::add_timeout(0.5, [](void*) {
        // Force quit the application
        Fl::awake();
        Fl::check();
        std::quick_exit(0); // Use quick_exit instead of exit
    }, nullptr);
}

void AppMainWindow::logoutUser() {
    // Save current data before logout
    portfolio.saveToFile(getPortfolioFilename());
    saveWishlistToFile();
    
    // Clear user data
    currentUser.clear();
    portfolio.clear();
    wishlist.clear();
    selectedStockIndices.clear();
    
    // Update welcome message
    welcomeLabel->copy_label("Hi, Guest!");
    
    // Hide main window
    hide();
    
    // Create new auth window instead of exiting
    AuthWindow* authWin = new AuthWindow(700, 500, "Stock Tracker - Authentication");
    authWin->show();
    
    // Wait for authentication
    while (authWin->shown()) {
        Fl::wait();
    }
    
    // If authenticated, show main window again
    if (authWin->isAuthenticated()) {
        setCurrentUser(authWin->getCurrentUser());
        show();
    } else {
        // Exit if not authenticated
        exit(0);
    }
    
    delete authWin;
}

bool AppMainWindow::validateUserPassword(const std::string& username, const std::string& password) {
    std::ifstream file("users.txt");
    if (!file.is_open()) return false;
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':', 0);
        if (pos != std::string::npos) {
            std::string storedUsername = line.substr(0, pos);
            std::string storedPassword = line.substr(pos + 1);
            if (username == storedUsername && password == storedPassword) {
                return true;
            }
        }
    }
    return false;
}

// Add the callbacks
void AppMainWindow::onLogoutClicked(Fl_Widget*, void* userdata) {
    auto* app = static_cast<AppMainWindow*>(userdata);
    app->logoutUser();
}

void AppMainWindow::onDeleteAccountClicked(Fl_Widget*, void* userdata) {
    auto* app = static_cast<AppMainWindow*>(userdata);
    app->deleteUserAccount();
}