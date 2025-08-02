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
#include <vector>
#include "StockData.h"
#include "DashboardTable.h"



class AppMainWindow : public Fl_Window {
    // Sidebar
    Fl_Group* sidebarGroup;
    Fl_Button* stockInfoBtn;
    Fl_Button* dashboardBtn;
    Fl_Button* tradeBtn;

    // Views
    Fl_Group* stockInfoGroup;
    Fl_Group* dashboardGroup;
    Fl_Group* tradeGroup;

    // Stock Info widgets
    Fl_Input* stockSymbolInput;
    Fl_Button* fetchButton;
    Fl_Button* addButton;
    Fl_Button* removeButton;
    Fl_Text_Display* stockOutputDisplay;
    Fl_Text_Buffer* stockTextBuffer;

    // Portfolio
    std::vector<StockData> portfolio;

    // Dashboard table
    DashboardTable* dashboardTable; // will be nullptr if not used
    Fl_Multiline_Output* dashboardOutput;

    // Helper methods
    void fetchStockAndAddToPortfolio();
    void switchTo(Fl_Group* target);
    void updateDashboardTable();

    // Callbacks
    static void onFetchClicked(Fl_Widget*, void*);
    static void onAddClicked(Fl_Widget*, void*);
    static void onRemoveClicked(Fl_Widget*, void*);
    static void onStockInfoBtnClicked(Fl_Widget*, void*);
    static void onDashboardBtnClicked(Fl_Widget*, void*);
    static void onTradeBtnClicked(Fl_Widget*, void*);

public:
    AppMainWindow(int w, int h, const char* title);
    ~AppMainWindow();
};
