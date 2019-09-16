#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "sdl.h"
#include <custom/timer_class.hpp>

int startTextInput();
void endTextInput();
void popChar();
const char* getText();
void addText(const char* str);
int checkTextInput();

#endif /* TEXTINPUT_H */
