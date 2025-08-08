#include <FL/Fl.H>
#include "AppMainWindow.h"
#include "AuthWindow.h"

int main(int argc, char** argv) {
    Fl::scheme("gtk+");
    
    // Show authentication window first (much larger size)
    AuthWindow authWin(700, 500, "Stock Tracker - Authentication");  // Much larger
    authWin.show();
    
    // Wait for authentication
    while (authWin.shown()) {
        Fl::wait();
    }
    
    // If authenticated, show main window
    if (authWin.isAuthenticated()) {
        AppMainWindow mainWin(1000, 600, "Stock Tracker");
        mainWin.setCurrentUser(authWin.getCurrentUser());
        mainWin.show();
        return Fl::run();
    } else {
        return 0; // Exit if not authenticated
    }
}
