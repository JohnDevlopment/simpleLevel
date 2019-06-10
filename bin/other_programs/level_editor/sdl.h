#ifndef SDL_H_INCLUDED
#define SDL_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <utility>

#include "program.h"

// initializes SDL; returns zero if successful
int initsdl(Program& pg);

// returns a SDL_Rect with the given values
SDL_Rect makerect(int x, int y, int w, int h);

#endif // SDL_H_INCLUDED
