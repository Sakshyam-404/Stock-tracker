#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Chart.H>
#include <vector>
#include <string>
#include <set>
#include "StockData.h"
#include "DashboardTable.h"
#include "StockAPI.h"
#include "LineChart.h"
#include "Portfolio.h"
#include "ChartData.h"


class AppMainWindow : public Fl_Window {
private:
    // Sidebar
    Fl_Group* sidebarGroup;
    Fl_Button* tradeBtn;
    Fl_Button* wishlistBtn;
    Fl_Button* portfolioBtn;
    Fl_Button* logoutBtn;
    Fl_Button* deleteAccountBtn;

    // Welcome message
    Fl_Box* welcomeLabel;

    // Views
    Fl_Group* tradeGroup;
    Fl_Group* wishlistGroup;
    Fl_Group* portfolioGroup;

    // Trade widgets
    Fl_Input* stockSymbolInput;
    Fl_Button* searchButton;
    Fl_Button* addButton;
    Fl_Button* buyButton;
    Fl_Button* sellButton;
    Fl_Input* quantityInput;
    Fl_Text_Display* stockOutputDisplay;
    Fl_Text_Buffer* stockTextBuffer;
    LineChart* lineChart;

    // Portfolio
    Portfolio portfolio;
    std::vector<StockData> wishlist;
    std::set<int> selectedStockIndices;
    std::string currentUser;

    // Wishlist
    Fl_Multiline_Output* wishlistOutput;
    Fl_Button* removeStockBtn;

    // Portfolio display
    Fl_Multiline_Output* portfolioOutput;
    
    // Helper methods
    std::string getPortfolioFilename() const { return currentUser + "_portfolio.txt"; }
    std::string getWishlistFilename() const { return currentUser + "_wishlist.txt"; }
    void fetchStockAndDisplay();
    void switchTo(Fl_Group* target);
    void updateWishlistTable();
    void updatePortfolioDisplay();
    void updateCandlestickChart(const std::string& symbol);
    void savePortfolioToFile();
    void loadPortfolioFromFile();
    void saveWishlistToFile();
    void loadWishlistFromFile();
    void removeSelectedStocks();
    void toggleStockSelection(int index);
    void executeTrade(bool isBuy);
    void deleteUserAccount();
    void showDeletionMessage();
    void logoutUser();
    bool validateUserPassword(const std::string& username, const std::string& password);
    
    // Callbacks
    static void onSearchClicked(Fl_Widget*, void*);
    static void onAddClicked(Fl_Widget*, void*);
    static void onBuyClicked(Fl_Widget*, void*);
    static void onSellClicked(Fl_Widget*, void*);
    static void onTradeBtnClicked(Fl_Widget*, void*);
    static void onWishlistBtnClicked(Fl_Widget*, void*);
    static void onPortfolioBtnClicked(Fl_Widget*, void*);
    static void onRemoveStockClicked(Fl_Widget*, void*);
    static void onWishlistOutputClicked(Fl_Widget*, void*);
    static void onDeleteAccountClicked(Fl_Widget*, void*);
    static void onLogoutClicked(Fl_Widget*, void*);
    static void onConfirmDeleteClicked(Fl_Widget*, void*);

public:
    AppMainWindow(int w, int h, const char* title);
    ~AppMainWindow();
    void setCurrentUser(const std::string& user);
    
    // Make these public for callback access
    friend void onDeleteAccountClicked(Fl_Widget*, void*);
    friend void onConfirmDeleteClicked(Fl_Widget*, void*);
    friend void onLogoutClicked(Fl_Widget*, void*);
};
