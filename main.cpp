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
#include "database_manager.hpp"
#include <mysql/mysql.h>

int choice = 0;
int menu = 1; // Start with the menu
int hindiGame;
int manipuriGame;
int playon;

int main(int argc, char **argv)
{
  if (mysql_library_init(0, NULL, NULL))
  {
    fprintf(stderr, "Could not initialize MySQL library\n");
    exit(1);
  }
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Ball Word Game");

  bool fullScreen = true; // Set to true for full screen, false for windowed
  if (fullScreen)
  {
    glutFullScreen(); // Set to full screen
    windowWidth = glutGet(GLUT_SCREEN_WIDTH);
    windowHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glViewport(0, 0, windowWidth, windowHeight);
  }

  init();
  glutMainLoop();

  // Cleanup FreeType
  FT_Done_Face(manipuriFace);
  FT_Done_Face(latinFace);
  FT_Done_FreeType(ft);
  mysql_library_end();

  return 0;
}
