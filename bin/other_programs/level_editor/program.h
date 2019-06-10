#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include <SDL.h>

struct Program {
    SDL_Window*     window;
    SDL_Renderer*   renderer;
    SDL_Surface*    screen;

    Program() : window(nullptr), renderer(nullptr), screen(nullptr) {}

    void destroy() {
        SDL_DestroyWindow(window);
        window      = nullptr;
        renderer    = nullptr;
        screen      = nullptr;
    }
};

#endif // PROGRAM_H_INCLUDED
