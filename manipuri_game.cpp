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
#include "manipuri_game.h"
#include "common.h"
#include "database_manager.hpp"

int highScore = 0;
#include "common.hpp"

bool isManipuriChar(wchar_t c)
{
    // Meetei Mayek Unicode range: U+ABC0 to U+ABFF
    return (c >= 0xABC0 && c <= 0xABFF);
}

void initManipuri()
{
    glClearColor(0.53, 0.81, 0.92, 1.0); // Sky blue background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    srand(static_cast<unsigned>(time(0)));
    // Initialize FreeType and load font
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "Could not init FreeType library" << std::endl;
        exit(1);
    }

    // Load Manipuri font
    if (FT_New_Face(ft, "Noto-Sans-Meetei-Mayek/NotoSansMeeteiMayek-Regular.ttf", 0, &manipuriFace))
    {
        std::cerr << "Could not load Manipuri font" << std::endl;
        exit(1);
    }

    // Load Latin font
    if (FT_New_Face(ft, "noto-sans-devanagari/NotoSansDevanagari-Regular.ttf", 0, &latinFace))
    {
        std::cerr << "Could not load Latin font" << std::endl;
        exit(1);
    }

    FT_Set_Pixel_Sizes(manipuriFace, 0, 48);
    FT_Set_Pixel_Sizes(latinFace, 0, 48);

    initkeyboardMapManipuri();

    restartButtonX = windowWidth / 2 - 450;
    restartButtonY = windowHeight / 2 - 60;
    quitButtonX = windowWidth / 2 + 150;
    quitButtonY = windowHeight / 2 - 60;

    backspaceIconX = windowWidth - backspaceIconSize - 10;
    backspaceIconY = 10;

    homeIconX = windowWidth - homeIconSize - 10;
    homeIconY = 10;

    lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000;
    glutTimerFunc(1000, updateTimer, 0);

    fail = false;
    gameOver = false;
    score = 0;
    level = 1;
    currentBallSpeed = 2.0f;
    remainingTime = 60;

    loadWordsFromFile(level);
    selectWordAndAssignLetters();

    glutDisplayFunc(displayManipuri);
    glutPostRedisplay();
}

void initkeyboardMapManipuri()
{
    // Map ASCII characters to Manipuri Unicode characters
    keyboardMap['k'] = L'ꯀ'; // Ka
    keyboardMap['s'] = L'ꯁ'; // Sa
    keyboardMap['l'] = L'ꯂ'; // La
    keyboardMap['m'] = L'ꯃ'; // Ma
    keyboardMap['p'] = L'ꯄ'; // Pa
    keyboardMap['n'] = L'ꯅ'; // Na
    keyboardMap['c'] = L'ꯆ'; // Cha
    keyboardMap['t'] = L'ꯇ'; // Ta
    keyboardMap['K'] = L'ꯈ'; // Kha
    keyboardMap['N'] = L'ꯉ'; // Nga
    keyboardMap['T'] = L'ꯊ'; // Tha
    keyboardMap['w'] = L'ꯋ'; // Wa
    keyboardMap['y'] = L'ꯌ'; // Ya
    keyboardMap['h'] = L'ꯍ'; // Ha
    keyboardMap['U'] = L'ꯎ'; // U
    keyboardMap['I'] = L'ꯏ'; // I
    keyboardMap['o'] = L'ꯑ'; // O
    keyboardMap['r'] = L'ꯔ'; // Ra
    keyboardMap['g'] = L'ꯒ'; // Ga
    keyboardMap['j'] = L'ꯖ'; // Ja
    keyboardMap['G'] = L'ꯗ'; // Gha
    keyboardMap['C'] = L'ꯚ'; // Pha
    keyboardMap['D'] = L'ꯙ'; // Dha
    keyboardMap['b'] = L'ꯕ'; // Ba
    keyboardMap['B'] = L'ꯗ'; // Bha
    keyboardMap['J'] = L'ꯠ'; // Jha
    keyboardMap['d'] = L'ꯗ'; // Da
    keyboardMap['z'] = L'ꯖ'; // Ja

    // Add vowels and diacritics if needed
    keyboardMap['a'] = L'ꯑ'; // A (Independent vowel)
    keyboardMap['e'] = L'ꯑ'; // E (Independent vowel)
    keyboardMap['i'] = L'ꯏ'; // I (Independent vowel)
    keyboardMap['u'] = L'ꯎ'; // U (Independent vowel)

    // Additional mappings can be added here for a more complete set.
}

void displayManipuri()
{

    glClear(GL_COLOR_BUFFER_BIT);

    if (fail)
    {
        playon = 0;
        glColor3f(1.0, 0.0, 0.0); // Red color
        renderText(windowWidth / 2.0 - 300, windowHeight / 2.0 + 300, L"ꯁꯥꯟꯅꯕ ꯂꯣꯏꯔꯦ! ꯄꯣꯏꯟꯇ : " + std::to_wstring(score));

        glColor3f(1.0, 1.0, 1.0); // White color
        renderText(windowWidth / 2.0 - 150, windowHeight / 2.0 + 120, L"ꯆꯨꯝꯕ ꯋꯥꯍꯩ : " + currentWord);
        renderText(windowWidth / 2.0 - 220, windowHeight / 2.0 + 50, L"ꯁꯦꯝꯒꯠꯂꯤꯕ ꯋꯥꯍꯩ :" + userInput);

        drawRectangle(restartButtonX, restartButtonY, restartButtonX + buttonWidth, restartButtonY + buttonHeight, 0.0f, 1.0f, 0.0f); // Green button
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(restartButtonX + buttonWidth * 0.5 - 115, restartButtonY + buttonHeight * 0.5 - 12, L"ꯑꯃꯨꯛ ꯍꯧꯕ");

        drawRectangle(quitButtonX, quitButtonY, quitButtonX + buttonWidth, quitButtonY + buttonHeight, 1.0f, 0.0f, 0.0f); // Red button
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(quitButtonX + buttonWidth * 0.5 - 55, quitButtonY + buttonHeight * 0.5 - 12, L"ꯊꯣꯛꯄ");

        glColor3f(1.0, 1.0, 0.0); // Yellow color for high score
        if (score > highScore)
        {
            highScore = score;
            dbManager.updateHighScore(loggedInUsername, highScore);
            renderText(windowWidth / 2.0 - 200, windowHeight / 2.0 + 210, L"ꯅꯨꯏ ꯁ꯭ꯀꯣꯔ: " + std::to_wstring(highScore)); // नया उच्च स्कोर
        }
        else
        {
            renderText(windowWidth / 2.0 - 150, windowHeight / 2.0 + 210, L"ꯍꯥꯏꯖꯦꯠ ꯁ꯭ꯀꯣꯔ: " + std::to_wstring(highScore)); // उच्च स्कोर
        }
    }
    else if (gameOver)
    {
        playon = 0;
        glColor3f(1.0, 0.0, 0.0); // Red color
        renderText(windowWidth / 2.0 - 200, windowHeight / 2.0 + 420, L"ꯑꯔꯣꯏꯕ ꯊꯥꯛ ꯌꯧꯔꯦ");
        renderText(windowWidth / 2.0 - 100, windowHeight / 2.0 + 300, L"ꯄꯣꯏꯟꯇ : " + std::to_wstring(score));

        drawRectangle(restartButtonX, restartButtonY, restartButtonX + buttonWidth, restartButtonY + buttonHeight, 0.0f, 1.0f, 0.0f); // Green button
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(restartButtonX + buttonWidth * 0.5 - 115, restartButtonY + buttonHeight * 0.5 - 12, L"ꯑꯃꯨꯛ ꯍꯧꯕ");

        drawRectangle(quitButtonX, quitButtonY, quitButtonX + buttonWidth, quitButtonY + buttonHeight, 1.0f, 0.0f, 0.0f); // Red button
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(quitButtonX + buttonWidth * 0.5 - 55, quitButtonY + buttonHeight * 0.5 - 12, L"ꯊꯣꯛꯄ");
        glColor3f(1.0, 1.0, 0.0); // Yellow color for high score
        if (score > highScore)
        {
            highScore = score;
            dbManager.updateHighScore(loggedInUsername, highScore);
            renderText(windowWidth / 2.0 - 200, windowHeight / 2.0 + 210, L"ꯅꯨꯏ ꯁ꯭ꯀꯣꯔ: " + std::to_wstring(highScore)); // नया उच्च स्कोर
        }
        else
        {
            renderText(windowWidth / 2.0 - 150, windowHeight / 2.0 + 210, L"ꯍꯥꯏꯖꯦꯠ ꯁ꯭ꯀꯣꯔ: " + std::to_wstring(highScore)); // उच्च स्कोर
        }
    }
    else
    {
        for (const Ball &ball : balls)
        {
            drawCircle(ball);
        }

        // Display forming word
        glColor3f(1.0, 1.0, 1.0); // White color for forming word
        renderText(10, windowHeight - 75, L"ꯁꯦꯝꯒꯠꯂꯤꯕ : " + userInput);

        // Display score, level, and remaining time
        renderText(10, windowHeight - 35, L"ꯄꯣꯏꯟꯇ : " + std::to_wstring(score));
        renderText(10, windowHeight - 115, L"ꯊꯥꯛ : " + std::to_wstring(level));
        renderText(10, windowHeight - 155, L"ꯃꯇꯝ ꯂꯦꯡꯂꯤꯕ : " + std::to_wstring(remainingTime));
    }

    // Always draw the backspace icon, regardless of game state
    if (playon)
        drawBackspaceIcon();
    else
        drawHomeIcon();

    glutSwapBuffers();
}
