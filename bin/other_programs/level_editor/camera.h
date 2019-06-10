#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <SDL.h>
#include <vector>
#include <custom/arrays.hpp>
#include <cstdio>

namespace camera {
    extern StaticDArray<SDL_Rect,3> rects; // back/front layer, level, sidebar
    extern bool                     scroll;
    extern SDL_Rect                 cam; // camera rect

    // define the dimensions of each camera rectangle
    void define(const SDL_Rect& src);

    // scroll the camera X-wise by [speed] speed
    void scrollx(int speed);

    // keep the camera within bounds
    void keepbound();
}

#endif // CAMERA_H_INCLUDED
