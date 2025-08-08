#pragma once
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <string>

class TradeDialog : public Fl_Window {
private:
    Fl_Input* quantityInput;
    Fl_Button* confirmButton;
    Fl_Button* cancelButton;
    Fl_Box* priceLabel;
    Fl_Box* totalLabel;
    
    std::string symbol;
    float price;
    bool isBuy;
    
    void updateTotal();
    static void onConfirmClicked(Fl_Widget*, void*);
    static void onCancelClicked(Fl_Widget*, void*);
    static void onQuantityChanged(Fl_Widget*, void*);
    
public:
    TradeDialog(int w, int h, const char* title, bool buy, const std::string& sym, float p);
    ~TradeDialog() {}
};
