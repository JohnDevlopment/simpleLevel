#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include "level.hpp"
#include "camera.h"
#include <arrays.hpp>
#include <image_class.hpp>
#include <vector>

#define GUI_CHK         0
#define GUI_BACKGROUND  1
#define GUI_ARROWS      2
#define GUI_MSG_OF_TI   3

extern Image* GUI;
extern int BGX;
extern SDL_Texture* SpriteTile;

void updateLSCRN();
void updateRSCRN();
SDL_Texture* loadTex(SDL_Renderer* ren, const char* file);
bool loadGlobalMedia(SDL_Renderer* ren);

#endif // GRAPHICS_H_INCLUDED
