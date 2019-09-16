#include <SDL.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
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
#include "textinput.h"

#define SEGCHECK()	std::cout << __FILE__ << ", line " << __LINE__ << ": checking for segmentation fault\n";
#define SEGEND()	std::cout << __FILE__ << ", line " << __LINE__ << ": no segmentation fault here\n";

using namespace std;

int str_isnum(const char* str);

static Program _program;

static int arguments(int argc, char* argv[]) {
    string cmd = "./fileExists ";
    
    // not enough args
    if (argc < 2) {
        cerr << "Not enough arguments, please add a file name\n";
        return 1;
    }

    // command string
    cmd += argv[1];
    
    // check if a file exists or not
    int result = system( cmd.c_str() );
    if (result != 0) {
        cerr << argv[1] << " does not exist\n";
        return 1;
    }

return 0;
}

static void quit() {
    cout << "quit()\n";
    
    {
        using namespace level;
        
        SDL_Free(Layers[0], Layers[1], Layers[2], Layers[3], Tileset);
        SDL_Free(_program.window, _program.renderer);
        
        Background->unload(); // unload background image
        delete[] Tiles;
    }

    IMG_Quit();
    SDL_Quit();
}

int main (int argc, char* argv[]) {
    using namespace level;
    
    generic_class<SDL_Event> event;
    int8_t i8Return;
    
    // parse arguments
    if (arguments(argc, argv)) {
        cerr << "Arguments are invalid\n";
        return 1;
    }
    
    // register system-wide cleanup function
    atexit(quit);
    
    /* Initialize SDL; load all graphical media;
       set the render draw color to black; and
       lastly, build the level tilemap and background image.
    */
    if (! initsdl(_program))
        return 1;
    if (! loadGlobalMedia(_program.renderer))
        return 1;
    SDL_SetRenderDrawColor(_program.renderer, 0, 0, 0, 255);    
    if (! level::build(argv[1], _program) )
        return 1;
    
    // dimensions of the sidebar
    StaticDArray<SDL_Rect,2> rects;
    
    {
        auto itr = rects.begin();
        
        // left screen
        itr->x = 0;
        itr->y = 0;
        itr->w = WIDTH;
        itr->h = HEIGHT;
        
        // right screen
        ++itr;
        itr->x = WIDTH;
        itr->y = 0;
        itr->w = 100;
        itr->h = HEIGHT;
    }
    
    // event loop
    i8Return = 0;
    
    while (! i8Return) {
        /* Process events: mainly processes mouse input with a couple of
           key inputs. If the return value is not zero, this loop will break.
        */
        i8Return = eventMain(event.getp());
        
        // check the status of the text input timer
        if ( checkTextInput() ) {
            // convert text input to an integer
            const char* text = getText();
            if ( str_isnum(text) ) {
                int temp = atoi(text);
                SpriteID = temp;
            }
            
            // end text input
            endTextInput();
            cout << "What we have: " << getText() << "\nSpriteID = " << (int) SpriteID << endl;
        }
        
        // fill the render area with the specified draw color
        SDL_RenderClear(_program.renderer);
        
        // update graphics on the left screen (see above: § "left screen")
        SDL_RenderSetViewport(_program.renderer, &rects[0]);
        updateLSCRN();
        
        // update graphics on the right screen (see above: § "right screen")
        SDL_RenderSetViewport(_program.renderer, &rects[1]);
        updateRSCRN();
        
        // render all graphics and reset the drawing area
        SDL_RenderSetViewport(_program.renderer, nullptr);
        SDL_RenderPresent(_program.renderer);
    }
    
    {
      int byteswritten = Level_WriteFile(argv[1], Header.get(), Tiles, (SprObj*) Sprites.data());
      printf("Bytes written to %s : %d\n", argv[1], byteswritten);
    }
    
return 0;
}

int str_isnum(const char* str) {
    int iNum = 0;
    int iLen = strlen(str);
    
    for (; iNum < iLen; ++iNum) {
        if ( ! isdigit(*str) )
            break;
        
        ++str;
    }
    
return iNum;
}
