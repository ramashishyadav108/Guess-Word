#ifndef COMMON_HPP
#define COMMON_HPP

#include <GL/glut.h>
#include <string>
#include <vector>
#include "database_manager.hpp"

// Forward declaration of Ball struct
struct Ball;

extern int windowWidth;
extern int windowHeight;
extern bool showLoginForm;
extern bool showSignupForm;
extern std::string inputUsername;
extern std::string inputPassword;
extern bool isLoggedIn;
extern std::string loggedInUsername;
extern DatabaseManager dbManager;
extern std::vector<Ball> balls;
void init();
void display();
void mouse(int button, int state, int x, int y);

void drawLoginForm();
void drawSignupForm();

#endif // COMMON_HPP
