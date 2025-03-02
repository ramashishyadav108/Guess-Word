#include <GL/glut.h>
#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <fstream>
#include <locale>
#include <codecvt>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "hindi_game.h"
#include "manipuri_game.h"
#include "common.h"
#include "common.hpp"
#include "database_manager.hpp" // Add this line

// Game variables
std::vector<Ball> balls;
std::vector<std::wstring> words; // Store Hindi words
std::wstring currentWord;
std::wstring userInput;
int score = 0;
bool fail = false;
bool gameOver = false;
float currentBallSpeed = 2.0f;          // Initial ball speed
const float speedIncreaseFactor = 1.2f; // Speed increase factor for each level
int remainingTime = 60;                 // Initial time for each level (in seconds)
int lastTime = 0;
int startLogIn = false;

// Window dimensions
int windowWidth = 800;
int windowHeight = 600;
const float ballRadius = 50.0f; // Ball radius
const float ballSpeed = 2.0f;   // Ball speed
int level = 1;                  // Track the current level
std::map<unsigned char, wchar_t> keyboardMap;

float backspaceIconX, backspaceIconY;
const float backspaceIconSize = 60.0f;

float homeIconX;
float homeIconY;
const float homeIconSize = 60.0f;

// Button dimensions and positions
const int buttonWidth = 300;
const int buttonHeight = 75;
int restartButtonX, restartButtonY;
int quitButtonX, quitButtonY;
int HindiButtonX, HindiButtonY;
int ManipuriButtonX, ManipuriButtonY;

int first = 1;

// FreeType variables
FT_Library ft;
FT_Face face;
FT_Face manipuriFace;
FT_Face latinFace;
FT_Face face2;

bool showLoginForm = false;
bool showSignupForm = false;
std::string inputUsername = "";
std::string inputPassword = "";
bool isLoggedIn = false;
std::string loggedInUsername = "";
DatabaseManager dbManager;

// New button positions
float loginButtonX, loginButtonY;
float signupButtonX, signupButtonY;
const float authButtonWidth = 200;
const float authButtonHeight = 50;

class TrieNode
{
public:
    std::unordered_map<wchar_t, std::unique_ptr<TrieNode>> children;
    bool isEndOfWord;

    TrieNode() : isEndOfWord(false) {}
};

std::string errorMessage = "";
std::string inputConfirmPassword = "";

InputField inputFocus = InputField::None;
class LanguageTrie
{
private:
    std::unique_ptr<TrieNode> root;

public:
    LanguageTrie() : root(std::make_unique<TrieNode>()) {}

    void insert(const std::wstring &word)
    {
        TrieNode *current = root.get();
        for (wchar_t ch : word)
        {
            if (current->children.find(ch) == current->children.end())
            {
                current->children[ch] = std::make_unique<TrieNode>();
            }
            current = current->children[ch].get();
        }
        current->isEndOfWord = true;
    }

    bool search(const std::wstring &word) const
    {
        const TrieNode *node = findNode(word);
        return (node != nullptr && node->isEndOfWord);
    }

    bool startsWith(const std::wstring &prefix) const
    {
        return (findNode(prefix) != nullptr);
    }

private:
    const TrieNode *findNode(const std::wstring &prefix) const
    {
        const TrieNode *current = root.get();
        for (wchar_t ch : prefix)
        {
            auto it = current->children.find(ch);
            if (it == current->children.end())
            {
                return nullptr;
            }
            current = it->second.get();
        }
        return current;
    }
};

// Global instance of LanguageTrie
LanguageTrie languageWordTrie;

void init()
{

    if (first)
    {
        glClearColor(1.0, 0.75, 0.8, 0.0); // pink color
        glMatrixMode(GL_PROJECTION);
        gluOrtho2D(0, windowWidth, 0, windowHeight);
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glutDisplayFunc(display);
        glutTimerFunc(0, timer, 0);
        glutMouseFunc(mouse);

        glutKeyboardFunc(keyboard); // Add keyboard function
    }
    else
    {
        glClearColor(1.0, 0.75, 0.8, 0.0); // pink color
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, windowWidth, 0, windowHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glutDisplayFunc(display);
        glutPostRedisplay();
    }

    menu = 1; // Start with the menu
    hindiGame = 0;
    manipuriGame = 0;
    playon = 0;

    // Initialize FreeType
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "Could not init FreeType library" << std::endl;
        exit(1);
    }

    // Load a font (ensure this path is correct and the font supports Devanagari)
    if (FT_New_Face(ft, "Rozha_One/RozhaOne-Regular.ttf", 0, &face2))
    {
        std::cerr << "Could not load font" << std::endl;
        exit(1);
    }
    FT_Set_Pixel_Sizes(face2, 0, 48); // Set font size

    // Calculate button positions based on current window size
    HindiButtonX = windowWidth / 2 - 450;
    HindiButtonY = windowHeight / 2 - 60;
    ManipuriButtonX = windowWidth / 2 + 150;
    ManipuriButtonY = windowHeight / 2 - 60;

    loginButtonX = windowWidth - 440;
    loginButtonY = windowHeight - 80;
    signupButtonX = windowWidth - 220;
    signupButtonY = windowHeight - 80;
}

void drawLoginForm()
{
    int formWidth = windowWidth / 3;
    int formHeight = windowHeight / 3;
    int baseX = (windowWidth - formWidth) / 2;
    int baseY = (windowHeight - formHeight) / 2;

    // Draw login form background
    drawRectangle(baseX, baseY, baseX + formWidth, baseY + formHeight, 0.2f, 0.2f, 0.2f);

    // Draw input fields and labels
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(baseX + 40, baseY + formHeight - 60, L"Username:");
    renderText(baseX + 40, baseY + formHeight - 140, L"Password:");

    drawRectangle(baseX + 40, baseY + formHeight - 100, baseX + formWidth - 40, baseY + formHeight - 65, 1.0f, 1.0f, 1.0f);
    drawRectangle(baseX + 40, baseY + formHeight - 180, baseX + formWidth - 40, baseY + formHeight - 145, 1.0f, 1.0f, 1.0f);

    glColor3f(0.0f, 0.0f, 0.0f);
    renderText(baseX + 45, baseY + formHeight - 95, std::wstring(inputUsername.begin(), inputUsername.end()));
    renderText(baseX + 45, baseY + formHeight - 175, std::wstring(inputPassword.length(), '*'));

    // Draw login button
    drawRectangle(baseX + formWidth / 2 - 75, baseY + 40, baseX + formWidth / 2 + 75, baseY + 90, 0.0f, 0.7f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(baseX + formWidth / 2 - 65, baseY + 55, L"Log In");

    // Display error message if any
    if (!errorMessage.empty())
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        renderText(baseX, baseY - 100, std::wstring(errorMessage.begin(), errorMessage.end()));
    }
}

void drawSignupForm()
{
    int formWidth = windowWidth / 3;
    int formHeight = windowHeight / 2.5;
    int baseX = (windowWidth - formWidth) / 2;
    int baseY = (windowHeight - formHeight) / 2;

    // Draw signup form background
    drawRectangle(baseX, baseY, baseX + formWidth, baseY + formHeight, 0.2f, 0.2f, 0.2f);

    // Draw input fields and labels
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(baseX + 40, baseY + formHeight - 60, L"Username:");
    renderText(baseX + 40, baseY + formHeight - 140, L"Password:");
    renderText(baseX + 40, baseY + formHeight - 220, L"Confirm Password:");

    drawRectangle(baseX + 40, baseY + formHeight - 100, baseX + formWidth - 40, baseY + formHeight - 65, 1.0f, 1.0f, 1.0f);
    drawRectangle(baseX + 40, baseY + formHeight - 180, baseX + formWidth - 40, baseY + formHeight - 145, 1.0f, 1.0f, 1.0f);
    drawRectangle(baseX + 40, baseY + formHeight - 260, baseX + formWidth - 40, baseY + formHeight - 225, 1.0f, 1.0f, 1.0f);

    glColor3f(0.0f, 0.0f, 0.0f);
    renderText(baseX + 45, baseY + formHeight - 95, std::wstring(inputUsername.begin(), inputUsername.end()));
    renderText(baseX + 45, baseY + formHeight - 175, std::wstring(inputPassword.length(), '*'));
    renderText(baseX + 45, baseY + formHeight - 255, std::wstring(inputConfirmPassword.length(), '*'));

    // Draw signup button
    drawRectangle(baseX + formWidth / 2 - 85, baseY + 40, baseX + formWidth / 2 + 85, baseY + 90, 0.0f, 0.0f, 0.7f);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(baseX + formWidth / 2 - 83, baseY + 55, L"Sign Up");

    // Display error message if any
    if (!errorMessage.empty())
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        renderText(baseX + 75, baseY - 100, std::wstring(errorMessage.begin(), errorMessage.end()));
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if (menu)
    {
        drawRectangle(loginButtonX, loginButtonY, loginButtonX + authButtonWidth, loginButtonY + authButtonHeight, 0.5f, 0.5f, 0.5f);
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(loginButtonX + 50, loginButtonY + 10, L"LogIn");

        drawRectangle(signupButtonX, signupButtonY, signupButtonX + authButtonWidth, signupButtonY + authButtonHeight, 0.5f, 0.5f, 0.5f);
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(signupButtonX + 25, signupButtonY + 10, L"SignUp");
    }

    if (menu && (!startLogIn))
    {
        glColor3f(1.0, 0.0, 0.0); // Red color
        renderText(windowWidth / 2.0 - 125, windowHeight / 2.0 + 310, L"Word Game");

        drawRectangle(HindiButtonX, HindiButtonY, HindiButtonX + buttonWidth, HindiButtonY + buttonHeight, 0.0f, 1.0f, 0.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(HindiButtonX + buttonWidth * 0.5 - 65, HindiButtonY + buttonHeight * 0.5 - 12, L"Hindi");

        drawRectangle(ManipuriButtonX, ManipuriButtonY, ManipuriButtonX + buttonWidth, ManipuriButtonY + buttonHeight, 1.0f, 0.0f, 0.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(ManipuriButtonX + buttonWidth * 0.5 - 95, ManipuriButtonY + buttonHeight * 0.5 - 12, L"Manipuri");
    }
    if (menu && (!isLoggedIn) && (!startLogIn))
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(windowWidth / 2 - 250, windowHeight / 2 + 170, L" Welcome to Home Page ");

        glColor3f(1.0, 1.0, 1.0); // White color
        renderText(windowWidth / 2.0 - 350, windowHeight / 2.0 + 75, L"First LogIn or SignUp to play Game");
    }
    if (isLoggedIn)
    {
        // Display logged in username
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(windowWidth / 2 - 300, windowHeight / 2 + 170, L"Hi " + std::wstring(loggedInUsername.begin(), loggedInUsername.end()) + L"  , Welcome to Home Page ");

        glColor3f(1.0, 1.0, 1.0); // White color
        renderText(windowWidth / 2.0 - 450, windowHeight / 2.0 + 75, L"Select your favourite language to play game");
    }

    if (menu && startLogIn)
    {
        // Display login form
        if (showLoginForm)
        {
            drawLoginForm();
        }

        // Display signup form
        if (showSignupForm)
        {
            drawSignupForm();
        }
    }

    glutSwapBuffers();
}

bool validateUserInput()
{
    return languageWordTrie.search(userInput);
}
void updateTimer(int value)
{
    if (playon && !fail && !gameOver)
    {
        int currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000;
        if (currentTime >= lastTime)
        {
            if (remainingTime <= 1)
            {
                remainingTime = 0;
                fail = true;
            }
            else
            {
                remainingTime--;
            }
            lastTime = currentTime;
        }
    }
    glutTimerFunc(1000, updateTimer, 0);
}

void drawBackspaceIcon()
{
    // Dark gray color for the icon
    glColor3f(0.2f, 0.2f, 0.2f);

    // Draw the main rectangle
    glBegin(GL_QUADS);
    glVertex2f(backspaceIconX, backspaceIconY);
    glVertex2f(backspaceIconX + backspaceIconSize, backspaceIconY);
    glVertex2f(backspaceIconX + backspaceIconSize, backspaceIconY + backspaceIconSize);
    glVertex2f(backspaceIconX, backspaceIconY + backspaceIconSize);
    glEnd();

    // Draw the "<-" symbol
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray for contrast
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    // Horizontal line
    glVertex2f(backspaceIconX + backspaceIconSize * 0.2f, backspaceIconY + backspaceIconSize * 0.5f);
    glVertex2f(backspaceIconX + backspaceIconSize * 0.8f, backspaceIconY + backspaceIconSize * 0.5f);
    // Left arrow part
    glVertex2f(backspaceIconX + backspaceIconSize * 0.4f, backspaceIconY + backspaceIconSize * 0.3f);
    glVertex2f(backspaceIconX + backspaceIconSize * 0.2f, backspaceIconY + backspaceIconSize * 0.5f);
    glVertex2f(backspaceIconX + backspaceIconSize * 0.4f, backspaceIconY + backspaceIconSize * 0.7f);
    glVertex2f(backspaceIconX + backspaceIconSize * 0.2f, backspaceIconY + backspaceIconSize * 0.5f);
    glEnd();
    glLineWidth(1.0f); // Reset line width
}
void drawHomeIcon()
{
    // Dark gray color for the icon background
    glColor3f(0.2f, 0.2f, 0.2f);

    // Draw the main square
    glBegin(GL_QUADS);
    glVertex2f(homeIconX, homeIconY);
    glVertex2f(homeIconX + homeIconSize, homeIconY);
    glVertex2f(homeIconX + homeIconSize, homeIconY + homeIconSize);
    glVertex2f(homeIconX, homeIconY + homeIconSize);
    glEnd();

    // Light gray color for the house symbol
    glColor3f(0.8f, 0.8f, 0.8f);
    glLineWidth(3.0f);

    // Draw the roof of the house
    glBegin(GL_TRIANGLES);
    glVertex2f(homeIconX + homeIconSize * 0.2f, homeIconY + homeIconSize * 0.6f);
    glVertex2f(homeIconX + homeIconSize * 0.5f, homeIconY + homeIconSize * 0.9f);
    glVertex2f(homeIconX + homeIconSize * 0.8f, homeIconY + homeIconSize * 0.6f);
    glEnd();

    // Draw the body of the house
    glBegin(GL_QUADS);
    glVertex2f(homeIconX + homeIconSize * 0.3f, homeIconY + homeIconSize * 0.6f);
    glVertex2f(homeIconX + homeIconSize * 0.7f, homeIconY + homeIconSize * 0.6f);
    glVertex2f(homeIconX + homeIconSize * 0.7f, homeIconY + homeIconSize * 0.2f);
    glVertex2f(homeIconX + homeIconSize * 0.3f, homeIconY + homeIconSize * 0.2f);
    glEnd();

    glLineWidth(1.0f); // Reset line width
}

void handleInput(wchar_t input, int ballIndex)
{
    if (input == L'\b')
    { // ASCII code for backspace
        if (!userInput.empty())
        {
            wchar_t lastChar = userInput.back();
            userInput.pop_back();
            // Find the last clicked ball with this letter and unclick it
            for (int i = balls.size() - 1; i >= 0; --i)
            {
                if (balls[i].letter[0] == lastChar && balls[i].clicked)
                {
                    balls[i].clicked = false;
                    balls[i].color[0] = static_cast<float>(rand()) / RAND_MAX;
                    balls[i].color[1] = static_cast<float>(rand()) / RAND_MAX;
                    balls[i].color[2] = static_cast<float>(rand()) / RAND_MAX;
                    break;
                }
            }
        }
    }
    else if (ballIndex != -1 && ballIndex < int(balls.size()))
    {
        Ball &ball = balls[ballIndex];
        if (!ball.clicked)
        {
            userInput += ball.letter;
            ball.clicked = true;
            ball.color[0] = 1.0f; // Change color to red
            ball.color[1] = 0.0f;
            ball.color[2] = 0.0f;
        }
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    if (showLoginForm || showSignupForm)
    {
        if (key == 13)
        { // Enter key
            // Handle form submission
            if (showLoginForm)
            {
                if (dbManager.loginUser(inputUsername, inputPassword))
                {
                    isLoggedIn = true;
                    loggedInUsername = inputUsername;
                    showLoginForm = false;
                    startLogIn = false; // Set startLogIn to false on successful login
                    errorMessage = "";
                }
                else
                {
                    errorMessage = "Invalid username or password";
                }
            }
            else if (showSignupForm)
            {
                if (inputPassword == inputConfirmPassword)
                {
                    if (dbManager.registerUser(inputUsername, inputPassword))
                    {
                        isLoggedIn = true;
                        loggedInUsername = inputUsername;
                        showSignupForm = false;
                        startLogIn = false; // Set startLogIn to false on successful signup
                        errorMessage = "";
                    }
                    else
                    {
                        errorMessage = "Registration failed. Username may already exist.";
                    }
                }
                else
                {
                    errorMessage = "Passwords do not match";
                }
            }
            inputUsername = "";
            inputPassword = "";
            inputConfirmPassword = "";
            inputFocus = InputField::None;
        }
        else if (key == 8)
        { // Backspace
            if (inputFocus == InputField::Username && !inputUsername.empty())
            {
                inputUsername.pop_back();
            }
            else if (inputFocus == InputField::Password && !inputPassword.empty())
            {
                inputPassword.pop_back();
            }
            else if (inputFocus == InputField::ConfirmPassword && !inputConfirmPassword.empty())
            {
                inputConfirmPassword.pop_back();
            }
        }
        else if (key >= 32 && key <= 126)
        { // Printable ASCII characters
            if (inputFocus == InputField::Username)
            {
                inputUsername += key;
            }
            else if (inputFocus == InputField::Password)
            {
                inputPassword += key;
            }
            else if (inputFocus == InputField::ConfirmPassword)
            {
                inputConfirmPassword += key;
            }
        }
        glutPostRedisplay();
    }
    else
    {
        if (key == 8)
        {                           // ASCII code for backspace
            handleInput(L'\b', -1); // Pass -1 as the ball index for backspace
        }
        else if (keyboardMap.find(key) != keyboardMap.end())
        {
            wchar_t languagechar = keyboardMap[key];
            // Find the first non-clicked ball with this letter
            for (size_t i = 0; i < balls.size(); ++i)
            {
                if (balls[i].letter[0] == languagechar && !balls[i].clicked)
                {
                    handleInput(languagechar, i);
                    break;
                }
            }
        }
    }
}
void drawCircle(const Ball &ball)
{
    glColor3fv(ball.color); // Use ball color
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ball.x, ball.y); // Center of circle
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(ball.x + cos(angle) * ballRadius, ball.y + sin(angle) * ballRadius);
    }
    glEnd();

    // Draw the letter on the circle
    glColor3f(0.0, 0.0, 0.0);                                         // Black color for text
    float textScale = 1.0f;                                           // Increased from 0.5f to 1.0f
    float textWidth = renderText(0, 0, ball.letter, textScale, true); // Measure text width
    float textX = ball.x - textWidth / 2;                             // Center text horizontally
    float textY = ball.y - (ballRadius / 2) * textScale;              // Center text vertically
    renderText(textX, textY, ball.letter, textScale);
}

void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void timer(int value)
{
    if (!fail && !gameOver)
    {
        // Update ball positions
        for (size_t i = 0; i < balls.size(); ++i)
        {
            balls[i].x += balls[i].dx;
            balls[i].y += balls[i].dy;

            // Bounce off the edges
            if (balls[i].x <= ballRadius || balls[i].x >= windowWidth - ballRadius)
            {
                balls[i].dx = -balls[i].dx;
                balls[i].x = std::max(ballRadius, std::min(balls[i].x, windowWidth - ballRadius));
            }
            if (balls[i].y <= ballRadius || balls[i].y >= windowHeight - ballRadius)
            {
                balls[i].dy = -balls[i].dy;
                balls[i].y = std::max(ballRadius, std::min(balls[i].y, windowHeight - ballRadius));
            }

            // Check for collisions with other balls
            for (size_t j = i + 1; j < balls.size(); ++j)
            {
                float dx = balls[j].x - balls[i].x;
                float dy = balls[j].y - balls[i].y;
                float distanceSquared = dx * dx + dy * dy;

                if (distanceSquared <= 4 * ballRadius * ballRadius)
                {
                    // Collision detected, resolve it
                    float distance = std::sqrt(distanceSquared);

                    // Normal vector
                    float nx = dx / distance;
                    float ny = dy / distance;

                    // Relative velocity
                    float dvx = balls[j].dx - balls[i].dx;
                    float dvy = balls[j].dy - balls[i].dy;

                    // Relative velocity along the normal
                    float velAlongNormal = dvx * nx + dvy * ny;

                    // Do not resolve if velocities are separating
                    if (velAlongNormal > 0)
                        continue;

                    // Collision impulse
                    float impulse = -2 * velAlongNormal;

                    // Apply impulse to the ball velocities
                    balls[i].dx -= impulse * nx;
                    balls[i].dy -= impulse * ny;
                    balls[j].dx += impulse * nx;
                    balls[j].dy += impulse * ny;

                    // Normalize velocities to maintain constant speed
                    for (Ball *ball : {&balls[i], &balls[j]})
                    {
                        float speed = std::sqrt(ball->dx * ball->dx + ball->dy * ball->dy);
                        if (speed != 0)
                        {
                            ball->dx = (ball->dx / speed) * currentBallSpeed;
                            ball->dy = (ball->dy / speed) * currentBallSpeed;
                        }
                    }

                    // Move balls apart to prevent sticking
                    float overlap = 2 * ballRadius - distance;
                    balls[i].x -= overlap * 0.5f * nx;
                    balls[i].y -= overlap * 0.5f * ny;
                    balls[j].x += overlap * 0.5f * nx;
                    balls[j].y += overlap * 0.5f * ny;
                }
            }
        }

        // Check if game is over or word is complete
        if (userInput.length() == currentWord.length() || validateUserInput())
        {
            if (userInput == currentWord)
            {
                score += currentWord.length() * 3; // Triple points for correct word
                level++;                           // Increment the level
                if (level > 5)
                {                    // Assuming 5 levels for this example
                    gameOver = true; // End the game if max level is reached
                }
                else
                {
                    loadWordsFromFile(level);
                    currentBallSpeed *= speedIncreaseFactor;      // Increase ball speed
                    remainingTime = 60 / level + 10;              // Reset timer for new level
                    lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000; // Reset last time
                    selectWordAndAssignLetters();
                }
            }
            else if (validateUserInput())
            {
                // Valid word, but not the target word
                score += userInput.length(); // Award points for valid word
                userInput.clear();
                for (auto &ball : balls)
                {
                    ball.clicked = false;
                    ball.color[0] = static_cast<float>(rand()) / RAND_MAX;
                    ball.color[1] = static_cast<float>(rand()) / RAND_MAX;
                    ball.color[2] = static_cast<float>(rand()) / RAND_MAX;
                }
            }
            else
            {
                fail = true; // Set fail state if the words don't match and it's not a valid word
            }
        }

        // Request to redraw the screen
        glutPostRedisplay();
    }
    glutPostRedisplay();
    // Call timer function again after 16 ms (~60 FPS)
    glutTimerFunc(16, timer, 0);
}
inline std::string trim(const std::string &s)
{
    std::string result = s;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch)
                                              { return !std::isspace(ch); }));
    return result;
}
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Convert mouse coordinates to window coordinates
        float mouseX = static_cast<float>(x);
        float mouseY = windowHeight - static_cast<float>(y);

        if (showLoginForm)
        {
            int formWidth = windowWidth / 3;
            int formHeight = windowHeight / 3;
            int baseX = (windowWidth - formWidth) / 2;
            int baseY = (windowHeight - formHeight) / 2;

            // Check if username field is clicked
            if (mouseX >= baseX + 40 && mouseX <= baseX + formWidth - 40 &&
                mouseY >= baseY + formHeight - 100 && mouseY <= baseY + formHeight - 65)
            {
                inputFocus = InputField::Username;
            }
            // Check if password field is clicked
            else if (mouseX >= baseX + 40 && mouseX <= baseX + formWidth - 40 &&
                     mouseY >= baseY + formHeight - 180 && mouseY <= baseY + formHeight - 145)
            {
                inputFocus = InputField::Password;
            }
            // Check if login button is clicked
            else if (mouseX >= baseX + formWidth / 2 - 75 && mouseX <= baseX + formWidth / 2 + 75 &&
                     mouseY >= baseY + 40 && mouseY <= baseY + 90)
            {
                if (dbManager.loginUser(inputUsername, inputPassword))
                {
                    isLoggedIn = true;
                    loggedInUsername = inputUsername;
                    startLogIn = false;
                    showLoginForm = false;
                    errorMessage = "";
                }
                else
                {
                    errorMessage = "Invalid username or password";
                }
                inputUsername = "";
                inputPassword = "";
                inputFocus = InputField::None;
                glutPostRedisplay();
                return; // Add this to prevent further processing
            }
        }

        // Handle signup form interactions
        if (showSignupForm)
        {
            int formWidth = windowWidth / 3;
            int formHeight = windowHeight / 2.5;
            int baseX = (windowWidth - formWidth) / 2;
            int baseY = (windowHeight - formHeight) / 2;

            // Check if username field is clicked
            if (mouseX >= baseX + 40 && mouseX <= baseX + formWidth - 40 &&
                mouseY >= baseY + formHeight - 100 && mouseY <= baseY + formHeight - 65)
            {
                inputFocus = InputField::Username;
            }
            // Check if password field is clicked
            else if (mouseX >= baseX + 40 && mouseX <= baseX + formWidth - 40 &&
                     mouseY >= baseY + formHeight - 180 && mouseY <= baseY + formHeight - 145)
            {
                inputFocus = InputField::Password;
            }
            // Check if confirm password field is clicked
            else if (mouseX >= baseX + 40 && mouseX <= baseX + formWidth - 40 &&
                     mouseY >= baseY + formHeight - 260 && mouseY <= baseY + formHeight - 225)
            {
                inputFocus = InputField::ConfirmPassword;
            }
            // Check if signup button is clicked
            else if (mouseX >= baseX + formWidth / 2 - 75 && mouseX <= baseX + formWidth / 2 + 75 &&
                     mouseY >= baseY + 40 && mouseY <= baseY + 90)
            {
                // Trim whitespace from inputs
                inputUsername = trim(inputUsername);
                inputPassword = trim(inputPassword);
                inputConfirmPassword = trim(inputConfirmPassword);

                if (inputUsername.empty() || inputPassword.empty() || inputConfirmPassword.empty())
                {
                    errorMessage = "Please fill in all fields";
                }
                else if (inputPassword != inputConfirmPassword)
                {
                    errorMessage = "Passwords do not match";
                }
                else
                {
                    if (dbManager.registerUser(inputUsername, inputPassword))
                    {
                        isLoggedIn = true;
                        loggedInUsername = inputUsername;
                        startLogIn = false;
                        showSignupForm = false;
                        errorMessage = "";
                    }
                    else
                    {
                        errorMessage = "Registration failed. Username may already exist.";
                    }
                }
                inputUsername = "";
                inputPassword = "";
                inputConfirmPassword = "";
                inputFocus = InputField::None;
                glutPostRedisplay();
                return; // Add this to prevent further processing
            }
        }

        if (menu)
        {

            // Check if login button is clicked
            if (mouseX >= loginButtonX && mouseX <= loginButtonX + authButtonWidth &&
                mouseY >= loginButtonY && mouseY <= loginButtonY + authButtonHeight)
            {
                showLoginForm = true;
                showSignupForm = false;
                isLoggedIn = false;
                startLogIn = true;
                errorMessage = "";
                glutPostRedisplay();
                return;
            }

            // Check if signup button is clicked
            if (mouseX >= signupButtonX && mouseX <= signupButtonX + authButtonWidth &&
                mouseY >= signupButtonY && mouseY <= signupButtonY + authButtonHeight)
            {
                showSignupForm = true;
                showLoginForm = false;
                isLoggedIn = false;
                startLogIn = true;
                errorMessage = "";
                glutPostRedisplay();
                return;
            }

            if (isLoggedIn)
            {
                if (mouseX >= HindiButtonX && mouseX <= HindiButtonX + buttonWidth * 2.3 && mouseY >= HindiButtonY && mouseY <= HindiButtonY + buttonHeight * 1.5)
                {
                    playon = 1;
                    hindiGame = 1;
                    menu = 0;
                    initHindi();
                    glutPostRedisplay();
                }
                // Check if quit button is clicked
                if (mouseX >= ManipuriButtonX && mouseX <= ManipuriButtonX + buttonWidth * 1.5 && mouseY >= ManipuriButtonY && mouseY <= ManipuriButtonY + buttonHeight * 1.5)
                {
                    playon = 1;
                    manipuriGame = 1;
                    menu = 0;
                    initManipuri();
                    glutPostRedisplay();
                }
            }
        }
        if ((!menu) && (!playon))
        {
            if (mouseX >= restartButtonX && mouseX <= restartButtonX + buttonWidth && mouseY >= restartButtonY && mouseY <= restartButtonY + buttonHeight)
            {
                playon = 1;
                restartGame();
            }

            // Check if quit button is clicked
            if (mouseX >= quitButtonX && mouseX <= quitButtonX + buttonWidth && mouseY >= quitButtonY && mouseY <= quitButtonY + buttonHeight)
            {
                exit(0); // Quit the game
            }

            if (mouseX >= homeIconX && mouseX <= homeIconX + homeIconSize &&
                mouseY >= homeIconY && mouseY <= homeIconY + homeIconSize)
            {

                menu = 1;
                hindiGame = 0;
                manipuriGame = 0;
                first = 0;
                init();
                glutPostRedisplay();
            }
        }

        if (playon)
        {
            if (mouseX >= backspaceIconX && mouseX <= backspaceIconX + backspaceIconSize &&
                mouseY >= backspaceIconY && mouseY <= backspaceIconY + backspaceIconSize)
            {
                handleInput(L'\b', -1); // Perform backspace action
                return;
            }
        }

        // Check if a ball is clicked
        for (size_t i = 0; i < balls.size(); ++i)
        {
            float dist = std::sqrt((mouseX - balls[i].x) * (mouseX - balls[i].x) + (mouseY - balls[i].y) * (mouseY - balls[i].y));
            if (dist < ballRadius)
            {
                if (!balls[i].clicked)
                {
                    handleInput(balls[i].letter[0], i);
                }
                break;
            }
        }

        // Check if restart button is clicked
        // Check if restart button is clicked
    }
}

void loadWordsFromFile(int level)
{
    words.clear();
    std::string filename = (hindiGame) ? "hindi/words" + std::to_string(level) + ".txt" : "manipuri_levels/words" + std::to_string(level) + ".txt";

    std::ifstream file(filename);
    if (file.is_open())
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string word;
        while (file >> word)
        {
            std::wstring wword = converter.from_bytes(word);
            words.push_back(wword);
            languageWordTrie.insert(wword); // Insert word into the Trie
        }
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

void selectWordAndAssignLetters()
{
    if (words.empty())
    {
        std::cerr << "No words available to select from." << std::endl;
        exit(1);
    }

    currentWord = words[rand() % words.size()];
    balls.clear();
    userInput.clear();

    for (wchar_t letter : currentWord)
    {
        Ball ball;
        ball.x = rand() % (windowWidth - 2 * static_cast<int>(ballRadius)) + ballRadius;
        ball.y = rand() % (windowHeight - 2 * static_cast<int>(ballRadius)) + ballRadius;
        ball.dx = (rand() % 2 == 0 ? currentBallSpeed : -currentBallSpeed);
        ball.dy = (rand() % 2 == 0 ? currentBallSpeed : -currentBallSpeed);
        ball.letter = std::wstring(1, letter);
        ball.clicked = false;
        ball.color[0] = static_cast<float>(rand()) / RAND_MAX;
        ball.color[1] = static_cast<float>(rand()) / RAND_MAX;
        ball.color[2] = static_cast<float>(rand()) / RAND_MAX;

        balls.push_back(ball);
    }
}

void restartGame()
{
    fail = false;
    gameOver = false;
    score = 0;
    level = 1;
    currentBallSpeed = 2.0f;                      // Reset ball speed to initial value
    remainingTime = 60;                           // Reset timer
    lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000; // Reset last time
    languageWordTrie = LanguageTrie();            // Reset the Trie
    loadWordsFromFile(level);
    selectWordAndAssignLetters();
}

float renderText(float x, float y, const std::wstring &text, float scale, bool measureOnly)
{
    float totalAdvance = 0;
    FT_GlyphSlot slot;

    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0f);

    for (const wchar_t &wc : text)
    {

        FT_Face currentFace;
        if (menu == 1)
            currentFace = face2;
        else if (hindiGame == 1)
            currentFace = face;
        else
            currentFace = isManipuriChar(wc) ? manipuriFace : latinFace;

        slot = currentFace->glyph;

        if (FT_Load_Char(currentFace, wc, FT_LOAD_RENDER))
        {
            std::wcerr << "Could not load character " << wc << std::endl;
            continue;
        }

        if (!measureOnly)
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_ALPHA,
                slot->bitmap.width,
                slot->bitmap.rows,
                0,
                GL_ALPHA,
                GL_UNSIGNED_BYTE,
                slot->bitmap.buffer);

            float x2 = totalAdvance + slot->bitmap_left;
            float y2 = -slot->bitmap_top;
            float w = slot->bitmap.width;
            float h = slot->bitmap.rows;

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(x2, -y2);
            glTexCoord2f(1, 0);
            glVertex2f(x2 + w, -y2);
            glTexCoord2f(1, 1);
            glVertex2f(x2 + w, -y2 - h);
            glTexCoord2f(0, 1);
            glVertex2f(x2, -y2 - h);
            glEnd();

            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
        }

        totalAdvance += (slot->advance.x >> 6);
    }

    glPopMatrix();

    return totalAdvance * scale;
}
