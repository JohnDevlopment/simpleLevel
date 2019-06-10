#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include "level.hpp"
#include "camera.h"
#include <custom/arrays.hpp>
#include <custom/image_class.hpp>
#include <vector>

#define GUI_CHK         0
#define GUI_BACKGROUND  1

namespace graphics {
    extern std::vector<Image>   gui;
    extern int                  bgx;
}

void updateLSCRN();

void updateRSCRN();

bool loadGlobalMedia(SDL_Renderer* ren);

#endif // GRAPHICS_H_INCLUDED
