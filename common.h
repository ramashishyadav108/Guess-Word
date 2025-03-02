#ifndef COMMON_H
#define COMMON_H

// Ball structure
struct Ball
{
    float x, y;
    float dx, dy;
    std::wstring letter;
    bool clicked;
    float color[3];
};

// Declare variables as extern
extern std::vector<Ball> balls;
extern std::vector<std::wstring> words;
extern std::wstring currentWord;
extern std::wstring userInput;
extern int score;
extern bool fail;
extern bool gameOver;
extern float currentBallSpeed;
extern int remainingTime;
extern int lastTime;
extern int windowWidth;
extern int windowHeight;
extern int level;
extern int playon;
extern std::map<unsigned char, wchar_t> keyboardMap;
extern float backspaceIconX, backspaceIconY;
extern int restartButtonX, restartButtonY;
extern int quitButtonX, quitButtonY;
extern int HindiButtonX, HindiButtonY;
extern int ManipuriButtonX, ManipuriButtonY;
extern FT_Library ft;
extern FT_Face face;
extern FT_Face manipuriFace;
extern FT_Face latinFace;
extern const float backspaceIconSize;
extern const int buttonWidth;
extern const int buttonHeight;
extern int choice;
extern int menu;
extern FT_Face face2;
extern int hindiGame;
extern int manipuriGame;
extern int gamestate;
extern int first;
extern float homeIconX; // 10 pixels from the left edge
extern float homeIconY; // 70 pixels from the top edge
extern const float homeIconSize;
enum class InputField
{
    None,
    Username,
    Password,
    ConfirmPassword
};
extern InputField inputFocus;
extern std::string inputConfirmPassword;

// Function prototypes
void init();
void display();
void drawCircle(const Ball &ball);
void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b);
void timer(int value);
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void loadWordsFromFile(int level);
void selectWordAndAssignLetters();
void restartGame();
float renderText(float x, float y, const std::wstring &text, float scale = 1.0f, bool measureOnly = false);
bool validateUserInput();
void updateTimer(int value);
void drawBackspaceIcon();
void handleInput(wchar_t input, int ballIndex);
void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b);
void drawCircle(const Ball &ball);

void drawHomeIcon();

#endif // GAME_H
