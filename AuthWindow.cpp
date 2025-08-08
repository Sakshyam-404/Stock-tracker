#include "AuthWindow.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

AuthWindow::AuthWindow(int w, int h, const char* title)
    : Fl_Window(w, h, title), isLoginMode(true), currentUser("")
{
    int padding = 50;
    int inputWidth = 300;
    int inputHeight = 35;
    int buttonWidth = 120;
    int buttonHeight = 35;
    
    // Title
    titleLabel = new Fl_Box(padding, padding, w - 2*padding, 50, "Stock Tracker - Login");
    titleLabel->labelsize(20);
    titleLabel->labelfont(FL_BOLD);
    titleLabel->labelcolor(FL_BLUE);
    titleLabel->align(FL_ALIGN_CENTER);
    
    // Username label (above input)
    usernameLabel = new Fl_Box(padding, padding + 80, inputWidth, 20, "Username");
    usernameLabel->labelsize(14);
    usernameLabel->labelfont(FL_BOLD);
    
    // Username input (no label inside)
    usernameInput = new Fl_Input(padding, padding + 100, inputWidth, inputHeight);
    usernameInput->textsize(14);
    
    // Password label (above input)
    passwordLabel = new Fl_Box(padding, padding + 150, inputWidth, 20, "Password");
    passwordLabel->labelsize(14);
    passwordLabel->labelfont(FL_BOLD);
    
    // Password input (no label inside)
    passwordInput = new Fl_Input(padding, padding + 170, inputWidth, inputHeight);
    passwordInput->type(FL_SECRET_INPUT);
    passwordInput->textsize(14);
    // Remove Enter key callback to avoid errors
    
    // Confirm password label (above input)
    confirmPasswordLabel = new Fl_Box(padding, padding + 220, inputWidth, 20, "Confirm Password");
    confirmPasswordLabel->labelsize(14);
    confirmPasswordLabel->labelfont(FL_BOLD);
    confirmPasswordLabel->hide();
    
    // Confirm password input (no label inside)
    confirmPasswordInput = new Fl_Input(padding, padding + 240, inputWidth, inputHeight);
    confirmPasswordInput->type(FL_SECRET_INPUT);
    confirmPasswordInput->textsize(14);
    confirmPasswordInput->hide();
    
    // Login button (left side)
    loginButton = new Fl_Button(padding, padding + 290, buttonWidth, buttonHeight, "Login");
    loginButton->callback(onLoginClicked, this);
    loginButton->labelsize(14);
    loginButton->color(FL_GREEN);
    loginButton->labelcolor(FL_WHITE);
    
    // Signup button (right side)
    signupButton = new Fl_Button(padding + buttonWidth + 20, padding + 290, buttonWidth, buttonHeight, "Sign Up");
    signupButton->callback(onSignupClicked, this);
    signupButton->labelsize(14);
    signupButton->color(FL_BLUE);
    signupButton->labelcolor(FL_WHITE);
    
    // Status label
    statusLabel = new Fl_Box(padding, padding + 340, w - 2*padding, 30, "");
    statusLabel->labelsize(12);
    statusLabel->labelcolor(FL_RED);
    statusLabel->align(FL_ALIGN_CENTER);
    
    end();
    
    // Start in login mode
    switchToLoginMode();
}

void AuthWindow::switchToLoginMode() {
    isLoginMode = true;
    titleLabel->copy_label("Stock Tracker - Login");
    confirmPasswordLabel->hide();
    confirmPasswordInput->hide();
    statusLabel->copy_label("");

    // Ensure order: [Login][Sign Up] (Login on the left)
    if (loginButton->x() > signupButton->x()) {
        int lx = loginButton->x(), sx = signupButton->x(), y = loginButton->y();
        loginButton->position(sx, y);
        signupButton->position(lx, y);
    }
    redraw();
}

void AuthWindow::switchToSignupMode() {
    isLoginMode = false;
    titleLabel->copy_label("Stock Tracker - Signup");
    confirmPasswordLabel->show();
    confirmPasswordInput->show();
    statusLabel->copy_label("");

    // Ensure order: [Sign Up][Login] (Sign Up on the left)
    if (signupButton->x() > loginButton->x()) {
        int lx = loginButton->x(), sx = signupButton->x(), y = loginButton->y();
        loginButton->position(sx, y);
        signupButton->position(lx, y);
    }
    redraw();
}

void AuthWindow::onLoginClicked(Fl_Widget*, void* userdata) {
    auto* auth = static_cast<AuthWindow*>(userdata);
    
    if (auth->isLoginMode) {
        // Perform login
        std::string username = auth->usernameInput->value();
        std::string password = auth->passwordInput->value();
        
        if (username.empty() || password.empty()) {
            auth->statusLabel->copy_label("Please enter username and password.");
            return;
        }
        
        if (auth->validateCredentials(username, password)) {
            auth->currentUser = username;
            auth->statusLabel->copy_label("Login successful!");
            auth->statusLabel->labelcolor(FL_GREEN);
            auth->hide(); // Close auth window
        } else {
            auth->statusLabel->copy_label("Invalid username or password.");
            auth->statusLabel->labelcolor(FL_RED);
        }
    } else {
        // Switch to login mode
        auth->switchToLoginMode();
    }
}

void AuthWindow::onSignupClicked(Fl_Widget*, void* userdata) {
    auto* auth = static_cast<AuthWindow*>(userdata);
    
    if (auth->isLoginMode) {
        // Switch to signup mode
        auth->switchToSignupMode();
    } else {
        // Perform signup
        std::string username = auth->usernameInput->value();
        std::string password = auth->passwordInput->value();
        std::string confirmPassword = auth->confirmPasswordInput->value();
        
        if (username.empty() || password.empty()) {
            auth->statusLabel->copy_label("Please enter username and password.");
            return;
        }
        
        if (confirmPassword.empty()) {
            auth->statusLabel->copy_label("Please confirm your password.");
            return;
        }
        
        if (password != confirmPassword) {
            auth->statusLabel->copy_label("Passwords do not match.");
            return;
        }
        
        if (auth->userExists(username)) {
            auth->statusLabel->copy_label("Username already exists.");
            return;
        }
        
        auth->createUser(username, password);
        auth->currentUser = username;
        
        // Show bonus message
        std::string bonusMessage = "Dear " + username + ", you got $10,000 as your signup bonus!";
        auth->statusLabel->copy_label(bonusMessage.c_str());
        auth->statusLabel->labelcolor(FL_GREEN);
        
        // Just show the message, don't hide - let user continue
        auth->statusLabel->redraw();
        auth->redraw();
    }
}

bool AuthWindow::validateCredentials(const std::string& username, const std::string& password) {
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

bool AuthWindow::userExists(const std::string& username) {
    std::ifstream file("users.txt");
    if (!file.is_open()) return false;
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':', 0);
        if (pos != std::string::npos) {
            std::string storedUsername = line.substr(0, pos);
            if (username == storedUsername) {
                return true;
            }
        }
    }
    return false;
}

void AuthWindow::createUser(const std::string& username, const std::string& password) {
    // Save user credentials
    std::ofstream file("users.txt", std::ios::app);
    if (file.is_open()) {
        file << username << ":" << password << "\n";
        file.close();
    }
    
    // Create user portfolio with $10,000 initial balance
    std::string portfolioFile = username + "_portfolio.txt";
    std::ofstream portfolioFileStream(portfolioFile);
    if (portfolioFileStream.is_open()) {
        portfolioFileStream << "10000.00\n"; // Initial cash balance
        portfolioFileStream.close();
    }
    
    // Create empty wishlist file
    std::string wishlistFile = username + "_wishlist.txt";
    std::ofstream wishlistFileStream(wishlistFile);
    if (wishlistFileStream.is_open()) {
        wishlistFileStream.close(); // Empty file
    }
} 