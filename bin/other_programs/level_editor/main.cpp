#include <SDL.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <custom/image_class.hpp>
#include <custom/lvalue_rvalue_pointers.hpp>
#include <custom/pointvector>

#include "graphics.h"
#include "program.h"
#include "sdl.h"
#include "event.h"
#include "camera.h"
#include "cfunc.hpp"
#include "tmath.hpp"

using namespace std;

static Program _pg;

static int arguments(int argc, char* argv[]) {
    // not enough args
    if (argc < 2) {
        cerr << "Not enough arguments, please add a file name\n";
        return 1;
    }

    // command string
    string cmd = "./fileExists ";
    cmd += argv[1];

    // check if a file exists or not
    int result = system(cmd.c_str());
    if (result != 0) {
        cerr << argv[1] << " does not exist\n";
        return 1;
    }

return 0;
}

static void quit() {
    {
        using namespace level;
        SDL_DestroyTexture(layers[0]);
        SDL_DestroyTexture(layers[1]);
        SDL_DestroyTexture(layers[2]);
        SDL_DestroyTexture(tileset);

        bg->unload(); // unload background image
        vtiles.clear(); // free vector array

        using graphics::gui;
        gui.clear();
    }

    IMG_Quit();
    SDL_Quit();
}

int main (int argc, char* argv[]) {
    generic_class<SDL_Event> event;

    // parse arguments
    if (arguments(argc, argv)) {
        cerr << "Arguments are invalid\n";
        return 1;
    }

    // register system-wide cleanup function
    atexit(quit);

    // initialize SDL2
    if (initsdl(_pg)) return 1;

    // load images for the side bar
    if (! loadGlobalMedia(_pg.renderer)) return 1;

    // set the renderer's draw color
    SDL_SetRenderDrawColor(_pg.renderer, 0, 0, 0, 255);

    // build level
    if (level::build(argv[1], _pg)) return 1;

    // dimensions of the sidebar
    StaticDArray<SDL_Rect,2> rects;

    {
        // left screen
        auto itr = rects.begin();
        itr->x   = 0;
        itr->y   = 0;
        itr->w   = WIDTH;
        itr->h   = HEIGHT;

        // right screen
        ++itr;
        itr->x = WIDTH;
        itr->y = 0;
        itr->w = 100;
        itr->h = HEIGHT;
    }

    // event loop
    int8_t i8Return = 0;

    while (! i8Return) {
        // process events
        i8Return = eventMain(event.get());

        // clear screen
        SDL_RenderClear(_pg.renderer);

        // update graphics on the left screen
        SDL_RenderSetViewport(_pg.renderer, &rects[0]);
        updateLSCRN();

        // update graphics on the right screen
        SDL_RenderSetViewport(_pg.renderer, &rects[1]);
        updateRSCRN();

        // render all graphics and reset the drawing area
        SDL_RenderSetViewport(_pg.renderer, nullptr);
        SDL_RenderPresent(_pg.renderer);
    }

    // report how many bytes were written to file
    cout << "Bytes written to " << argv[1] << " : " \
    << Level_WriteFile(argv[1], level::header.get(), level::vtiles) \
    << endl;

return 0;
}
