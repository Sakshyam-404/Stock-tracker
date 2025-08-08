#pragma once
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <string>

class AuthWindow : public Fl_Window {
private:
    Fl_Input* usernameInput;
    Fl_Input* passwordInput;
    Fl_Input* confirmPasswordInput;
    Fl_Box* usernameLabel;
    Fl_Box* passwordLabel;
    Fl_Box* confirmPasswordLabel;
    Fl_Button* loginButton;
    Fl_Button* signupButton;
    Fl_Box* titleLabel;
    Fl_Box* statusLabel;
    
    bool isLoginMode;
    std::string currentUser;
    
    void switchToLoginMode();
    void switchToSignupMode();
    bool validateCredentials(const std::string& username, const std::string& password);
    bool userExists(const std::string& username);
    void createUser(const std::string& username, const std::string& password);
    
    static void onLoginClicked(Fl_Widget*, void*);
    static void onSignupClicked(Fl_Widget*, void*);
    // Removed onEnterKeyPressed to avoid errors
    
public:
    AuthWindow(int w, int h, const char* title);
    ~AuthWindow() {}
    
    std::string getCurrentUser() const { return currentUser; }
    bool isAuthenticated() const { return !currentUser.empty(); }
}; 