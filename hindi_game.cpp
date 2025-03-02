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
#include "common.h"
#include "database_manager.hpp"

int high_score = 0;
#include "common.hpp"

void initHindi()
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
    if (FT_New_Face(ft, "Rozha_One/RozhaOne-Regular.ttf", 0, &face))
    {
        std::cerr << "Could not load font" << std::endl;
        exit(1);
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    initKeyboardMapHindi();

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
    high_score = dbManager.getHighScore(loggedInUsername);

    loadWordsFromFile(level);
    selectWordAndAssignLetters();

    glutDisplayFunc(displayHindi);
    glutPostRedisplay();
}

void initKeyboardMapHindi()
{
    // Map ASCII characters to Hindi Unicode characters
    keyboardMap['a'] = L'अ';
    keyboardMap['A'] = L'आ';
    keyboardMap['i'] = L'इ';
    keyboardMap['I'] = L'ई';
    keyboardMap['u'] = L'उ';
    keyboardMap['U'] = L'ऊ';
    keyboardMap['e'] = L'ए';
    keyboardMap['E'] = L'ऐ';
    keyboardMap['o'] = L'ओ';
    keyboardMap['O'] = L'औ';

    keyboardMap['k'] = L'क';
    keyboardMap['K'] = L'ख';
    keyboardMap['g'] = L'ग';
    keyboardMap['G'] = L'घ';
    keyboardMap['c'] = L'च';
    keyboardMap['C'] = L'छ';
    keyboardMap['j'] = L'ज';
    keyboardMap['J'] = L'झ';
    keyboardMap['T'] = L'ट';
    keyboardMap[';'] = L'ठ';
    keyboardMap['D'] = L'ड';
    keyboardMap[':'] = L'ढ';
    keyboardMap['N'] = L'ण';
    keyboardMap['t'] = L'त';
    keyboardMap['\\'] = L'थ';
    keyboardMap['d'] = L'द';
    keyboardMap['|'] = L'ध';
    keyboardMap['n'] = L'न';
    keyboardMap['p'] = L'प';
    keyboardMap['P'] = L'फ';
    keyboardMap['b'] = L'ब';
    keyboardMap['B'] = L'भ';
    keyboardMap['m'] = L'म';
    keyboardMap['y'] = L'य';
    keyboardMap['r'] = L'र';
    keyboardMap['l'] = L'ल';
    keyboardMap['v'] = L'व';
    keyboardMap['S'] = L'श';
    keyboardMap['s'] = L'स';
    keyboardMap['h'] = L'ह';

    // Map vowel matras
    keyboardMap['1'] = L'ि'; // ि (Short i)
    keyboardMap['2'] = L'ी'; // ी (Long i)
    keyboardMap['3'] = L'ु';  // ु (Short u)
    keyboardMap['4'] = L'ू';  // ू (Long u)
    keyboardMap['5'] = L'ृ';  // ृ (R with a matra)
    keyboardMap['6'] = L'े';  // े (Short e)
    keyboardMap['7'] = L'ै';  // ै (Long e)
    keyboardMap['-'] = L'ो'; // ो (Short o)
    keyboardMap['9'] = L'ौ'; // ौ (Long o)
    keyboardMap['0'] = L'ा';

    // Add more mappings as needed
}

void displayHindi()
{

    glClear(GL_COLOR_BUFFER_BIT);

    if (fail)
    {
        playon = 0;
        glColor3f(1.0, 0.0, 0.0); // Red color
        renderText(windowWidth / 2.0 - 150, windowHeight / 2.0 + 300, L"गेम ओवर! स्कोर: " + std::to_wstring(score));

        glColor3f(1.0, 1.0, 1.0); // White color
        renderText(windowWidth / 2.0 - 220, windowHeight / 2.0 + 50, L"बनाया गया शब्द:" + userInput);
        renderText(windowWidth / 2.0 - 150, windowHeight / 2.0 + 120, L" सही शब्द: " + currentWord);

        drawRectangle(restartButtonX, restartButtonY, restartButtonX + buttonWidth, restartButtonY + buttonHeight, 0.0f, 1.0f, 0.0f); // Green button
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(restartButtonX + buttonWidth * 0.5 - 115, restartButtonY + buttonHeight * 0.5 - 12, L"पुनः आरंभ करें");

        drawRectangle(quitButtonX, quitButtonY, quitButtonX + buttonWidth, quitButtonY + buttonHeight, 1.0f, 0.0f, 0.0f); // Red button
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(quitButtonX + buttonWidth * 0.5 - 55, quitButtonY + buttonHeight * 0.5 - 12, L"बंद करें");

        glColor3f(1.0, 1.0, 0.0); // Yellow color for high score
        if (score > high_score)
        {
            high_score = score;
            dbManager.updateHighScore(loggedInUsername, high_score);
            renderText(windowWidth / 2.0 - 200, windowHeight / 2.0 + 210, L"नया उच्च स्कोर: " + std::to_wstring(high_score));
        }
        else
        {
            renderText(windowWidth / 2.0 - 150, windowHeight / 2.0 + 210, L"उच्च स्कोर: " + std::to_wstring(high_score));
        }
    }
    else if (gameOver)
    {
        playon = 0;
        glColor3f(1.0, 0.0, 0.0); // Red color
        renderText(windowWidth / 2.0 - 230, windowHeight / 2.0 + 420, L"अधिकतम स्तर तक पहुँच गया ");
        renderText(windowWidth / 2.0, windowHeight / 2.0 + 300, L"स्कोर: " + std::to_wstring(score));

        drawRectangle(restartButtonX, restartButtonY, restartButtonX + buttonWidth, restartButtonY + buttonHeight, 0.0f, 1.0f, 0.0f); // Green button
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(restartButtonX + buttonWidth * 0.5 - 115, restartButtonY + buttonHeight * 0.5 - 12, L"पुनः आरंभ करें");

        drawRectangle(quitButtonX, quitButtonY, quitButtonX + buttonWidth, quitButtonY + buttonHeight, 1.0f, 0.0f, 0.0f); // Red button
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(quitButtonX + buttonWidth * 0.5 - 55, quitButtonY + buttonHeight * 0.5 - 12, L"बंद करें");

        glColor3f(1.0, 1.0, 0.0); // Yellow color for high score
        if (score > high_score)
        {
            high_score = score;
            dbManager.updateHighScore(loggedInUsername, high_score);
            renderText(windowWidth / 2.0 - 200, windowHeight / 2.0 + 210, L"नया उच्च स्कोर: " + std::to_wstring(high_score));
        }
        else
        {
            renderText(windowWidth / 2.0 - 150, windowHeight / 2.0 + 210, L"उच्च स्कोर: " + std::to_wstring(high_score));
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
        renderText(10, windowHeight - 75, L"बनाया जा रहा है : " + userInput);

        // Display score
        renderText(10, windowHeight - 35, L"स्कोर: " + std::to_wstring(score));
        renderText(10, windowHeight - 115, L"स्तर: " + std::to_wstring(level)); // Display current level

        glColor3f(1.0, 1.0, 1.0); // White color
        renderText(10, windowHeight - 155, L"समय शेष: " + std::to_wstring(remainingTime));
    }

    // Always draw the backspace icon, regardless of game state
    if (playon)
        drawBackspaceIcon();
    else
        drawHomeIcon();

    glutSwapBuffers();
}
