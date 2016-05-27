#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\System.hpp> 
#include <SFML\Window.hpp>

#define MENUWIDTH 1280
#define MENUHEIGHT 600
#define WINWIDTH MENUWIDTH
#define WINHEIGHT MENUHEIGHT

#define MEDIUMSTART 6
#define HARDSTART 10
#define NUMOFNONS 15
#define NUMOFZERO NUMOFNONS - 1

/* Initialize dynamic 2D array of integer numbers. */
int **CreateArr(int m, int n);

/* Number to string function. */
std::string N2S(int a);
