#include "sdl.h"
#include "cfunc.hpp"

using std::cerr;

SDL_Rect makerect(int x, int y, int w, int h) {
    SDL_Rect retval = {x, y, w, h};

return std::move(retval);
}

int initsdl(Program& pg) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "SDL_Init: " << SDL_GetError() << '\n';
        return 1;
    }

    // create window
    pg.window = SDL_CreateWindow("Level Editor", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, cadd(WIDTH, 100), HEIGHT, 0);
    if (pg.window == nullptr) {
        cerr << "SDL_CreateWindow: " << SDL_GetError() << '\n';
        return 1;
    }

    // create renderer
    pg.renderer = SDL_CreateRenderer(pg.window, -1,
                                     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (pg.renderer == nullptr) {
        cerr << "SDL_CreateRenderer: " << SDL_GetError() << '\n';
        return 1;
    }

    // SDL2_image
    constexpr int flags = IMG_INIT_PNG|IMG_INIT_JPG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        cerr << "IMG_Init: " << IMG_GetError() << '\n';
        return 1;
    }

return 0;
}
