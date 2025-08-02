#include "AppMainWindow.h"
#include "StockAPI.h"
#include <sstream>
#include "DashboardTable.h"

AppMainWindow::AppMainWindow(int w, int h, const char *title)
    : Fl_Window(w, h, title)
{
    this->resizable(this);

    int sidebarWidth = 200;
    int padding = 10;

    // Sidebar
    sidebarGroup = new Fl_Group(0, 0, sidebarWidth, h);
    stockInfoBtn = new Fl_Button(10, 20, 180, 40, "Stock Info");
    dashboardBtn = new Fl_Button(10, 70, 180, 40, "Dashboard");
    tradeBtn = new Fl_Button(10, 120, 180, 40, "Buy/Sell");
    stockInfoBtn->callback(onStockInfoBtnClicked, this);
    dashboardBtn->callback(onDashboardBtnClicked, this);
    tradeBtn->callback(onTradeBtnClicked, this);
    sidebarGroup->end();

    // Stock Info
    stockInfoGroup = new Fl_Group(sidebarWidth + 30, 0, w - sidebarWidth - 30, h);
    stockSymbolInput = new Fl_Input(sidebarWidth + padding + 30, 30, 200, 30, "Symbol:");
    stockSymbolInput->when(FL_WHEN_ENTER_KEY_ALWAYS);
    stockSymbolInput->callback(onFetchClicked, this); // Enter triggers search only
    fetchButton = new Fl_Button(sidebarWidth + 260 + 30, 30, 80, 30, "Search");
    fetchButton->callback(onFetchClicked, this);
    // Add button next to Search
    addButton = new Fl_Button(sidebarWidth + 350 + 30, 30, 80, 30, "Add");
    addButton->callback(onAddClicked, this);

    stockTextBuffer = new Fl_Text_Buffer();

    stockOutputDisplay = new Fl_Text_Display(sidebarWidth + padding + 30, 70, w - sidebarWidth - 2 * padding - 30, h - 100);
    stockOutputDisplay->buffer(stockTextBuffer);

    stockInfoGroup->end();

    // Dashboard
    dashboardGroup = new Fl_Group(sidebarWidth, 0, w - sidebarWidth, h);
    dashboardGroup->end();
    dashboardGroup->hide();
    // Remove DashboardTable and use plain text output
    dashboardOutput = new Fl_Multiline_Output(sidebarWidth + 10, 30, w - sidebarWidth - 20, h - 60);
    dashboardOutput->readonly(1);
    dashboardGroup->add(dashboardOutput);

    // Trade
    tradeGroup = new Fl_Group(sidebarWidth, 0, w - sidebarWidth, h);
    new Fl_Box(sidebarWidth + 100, 200, 300, 30, "Buy/Sell page coming soon...");
    tradeGroup->end();
    tradeGroup->hide();

    end();
    switchTo(stockInfoGroup);
}

AppMainWindow::~AppMainWindow() {
    // Do NOT manually delete FLTK widgets that are children of a window or group.
    // FLTK will handle their deletion automatically.
    // Only delete objects not managed by FLTK.
    // Remove dashboardTable if it was allocated
    if (dashboardTable) {
        delete dashboardTable;
        dashboardTable = nullptr;
    }
    // Remove dashboardOutput if allocated
    if (dashboardOutput) {
        delete dashboardOutput;
        dashboardOutput = nullptr;
    }
}

void AppMainWindow::switchTo(Fl_Group *target)
{
    stockInfoGroup->hide();
    dashboardGroup->hide();
    tradeGroup->hide();
    target->show();
    redraw();
}
void AppMainWindow::updateDashboardTable()
{
    // Print plain text, no formatting
    std::ostringstream oss;
    for (const auto& s : portfolio) {
        oss << s.getSymbol() << " " << s.getPrice() << " " << s.getChange() << " " << s.getChangePercent() << " " << s.getHigh() << " " << s.getLow() << "\n";
    }
    dashboardOutput->value(oss.str().c_str());
}

void AppMainWindow::onAddClicked(Fl_Widget *, void *userdata)
{
    auto *app = static_cast<AppMainWindow *>(userdata);
    std::string symbol = app->stockSymbolInput->value();
    if (symbol.empty())
        return;

    StockData stock = StockAPI::fetchStock(symbol);
    // Check for invalid stock (empty symbol or price 0)
    if (stock.getSymbol().empty() || stock.getPrice() == 0.0f) {
        app->stockTextBuffer->text("Invalid stock symbol or not found.\n");
        return;
    }
    // Prevent duplicate adding
    for (const auto& s : app->portfolio) {
        if (s.getSymbol() == stock.getSymbol()) {
            app->stockTextBuffer->text("Stock already in dashboard.\n");
            return;
        }
    }
    app->portfolio.push_back(stock);
    app->updateDashboardTable();
    // Switch to dashboard after adding
    app->switchTo(app->dashboardGroup);
}

void AppMainWindow::onFetchClicked(Fl_Widget *, void *userdata)
{
    auto *app = static_cast<AppMainWindow *>(userdata);
    app->fetchStockAndAddToPortfolio();
}

void AppMainWindow::fetchStockAndAddToPortfolio()
{
    std::string symbol = stockSymbolInput->value();
    if (symbol.empty())
        return;

    StockData stock = StockAPI::fetchStock(symbol);
    // Check for invalid stock (empty symbol or price 0)
    if (stock.getSymbol().empty() || stock.getPrice() == 0.0f) {
        stockTextBuffer->text("Invalid stock symbol or not found.\n");
        return;
    }
    // Only display info, do NOT add to portfolio here

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

    // ✅ Just use plain font and size
    stockOutputDisplay->textfont(FL_HELVETICA_BOLD);
    stockOutputDisplay->textsize(16); // Adjust if too large for screen
}

void AppMainWindow::onStockInfoBtnClicked(Fl_Widget *, void *userdata)
{
    static_cast<AppMainWindow *>(userdata)->switchTo(static_cast<AppMainWindow *>(userdata)->stockInfoGroup);
}

void AppMainWindow::onDashboardBtnClicked(Fl_Widget *, void *userdata)
{
    static_cast<AppMainWindow *>(userdata)->switchTo(static_cast<AppMainWindow *>(userdata)->dashboardGroup);
}

void AppMainWindow::onTradeBtnClicked(Fl_Widget *, void *userdata)
{
    static_cast<AppMainWindow *>(userdata)->switchTo(static_cast<AppMainWindow *>(userdata)->tradeGroup);
}
