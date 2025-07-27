//// StockApp.cpp
//#include "StockApp.h"
//#include <sstream>
//
//StockApp::StockApp(int x, int y, int w, int h) {
//    group = new Fl_Group(x, y, w, h);
//
//    input = new Fl_Input(x + 60, y + 20, 200, 30, "Symbol:");
//    button = new Fl_Button(x + 270, y + 20, 80, 30, "Fetch");
//    output = new Fl_Multiline_Output(x + 20, y + 70, w - 40, h - 90);
//
//    button->callback(onFetchedClicked, this);
//    group->end();
//}
//
//void StockApp::onFetchedClicked(Fl_Widget*, void* userdata) {
//    StockApp* app = static_cast<StockApp*>(userdata);
//    app->fetchAndDisplay();
//}
//
//void StockApp::fetchAndDisplay() {
//    std::string symbol = input->value();
//    if (symbol.empty()) return;
//
//    StockData stock = StockAPI::fetchStock(symbol);
//    portfolio.addStock(stock);
//
//    std::ostringstream oss;
//    oss << "Fetched and added:\n\n";
//    oss << "Symbol: " << stock.getSymbol() << "\n"
//        << "Price: " << stock.getPrice() << "\n"
//        << "Change: " << stock.getChange() << "\n"
//        << "Change Percent: " << stock.getChangePercent() << "%\n"
//        << "High: " << stock.getHigh() << "\n"
//        << "Low: " << stock.getLow() << "\n";
//
//    output->value(oss.str().c_str());
//}
//
//Fl_Group* StockApp::getGroup() const {
//    return group;
//}
//
//const Portfolio& StockApp::getPortfolio() const {
//    return portfolio;
//}
