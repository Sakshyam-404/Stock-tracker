#include <FL/Fl.H>
#include "AppMainWindow.h"

int main(int argc, char** argv) {
    Fl::scheme("gtk+"); 
    AppMainWindow win(1000, 600, "Stock Viewer");
    win.show();
    return Fl::run();
}
