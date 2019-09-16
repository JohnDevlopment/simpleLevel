#include "event.h"
#include "level.hpp"
#include "camera.h"
#include "graphics.h"
#include "textinput.h"

static uint8_t keymasks = 0;

static int _mouse_over_left_screen(SDL_Event& event) {
    using namespace level;
    
    /* Tile at (x,y) where mouse was clicked. X coordinate is offset
       by the X coordinate of the camera. */
    uint16_t desttile = gettile(Header->width, event.button.x + camera::Cam.x,
                                event.button.y);

    // tile selection mode
    if (Flags.mask(FLAGS_TILE_SEL)) {
        //const SDL_Rect tsdst = {192, 112, 256, 256};
        // point of mouse click relative to the image displayed
        Point<int> mouse(event.button.x - 192, event.button.y - 112);

        // valid position
        if (mouse < 256 && mouse >= 0) {
            switch (event.button.button) {
            default:
                break;

            case SDL_BUTTON_LEFT:
                {
                    TileNumbers[0] = gettile(16, mouse.x(), mouse.y());
                    printTile();
                    break;
                }

            case SDL_BUTTON_RIGHT:
                {
                    TileNumbers[1] = gettile(16, mouse.x(), mouse.y());
                    printTile();
                    break;
                }
            }
        } // end if -- mouse within bounds
    } // end if -- tile selection mode

    // fill-line mode enabled
    else if (Flags.mask(FLAGS_FILL_BOTH)) {
        // which stage of fill-line mode are we in?
        char cFirstLast = (char) Flags.get() & FLAGS_FILL_BOTH;

        // starting phase
        if (cFirstLast == FLAGS_FILL_ST) {
            // mark the first tile in a straight line; initiate the next stage
            TileNumbers[2] = desttile;
            Flags.unset(FLAGS_FILL_ST);
            Flags.set(FLAGS_FILL_END);
        }

        // final phase
        else {
            // mark the last tile in the line; turn off fill-line mode
            TileNumbers[3] = desttile;
            Flags.unset(FLAGS_FILL_END);
            Flags.set(FLAGS_FILL_NULL);

            // fill each tile in the line with the selected tile number
            changeLine(Background->renderer);
        }
    }
    
    // sprite selection mode
    else if ( Flags.mask(FLAGS_SPRITE_SEL) ) {
        // left-click
        if (event.button.button == SDL_BUTTON_LEFT) {
            changeSpriteTile(Background->renderer, desttile, false);
        }
        // right-click
        else {
            changeSpriteTile(Background->renderer, desttile, true);
        }
    }
    
    // paste a single tile onto the level
    else {
        // right-click a tile
        if (event.button.button == SDL_BUTTON_RIGHT) {
            TileNumbers[0] = Tiles[desttile].graphicID;
            TileNumbers[1] = Tiles[desttile].codeID;
            printTile();
        }
        // left click a tile
        else
            changeTile(Background->renderer, desttile);
    }

return 0;
}

static int _mousedown(SDL_Event& event) {
    using level::Flags;

    // index to iLyY; which box to check
    uint8_t uiLayer = Flags.mask(FLAGS_CHK_LAYER);

    // Y offset for coll: box to draw check mark on
    int iLyY[] = {144, 120};

    // collision rectangle of layer check box
    SDL_Rect coll = {7 + WIDTH, 0, 15, 15};
    coll.y = iLyY[uiLayer];

    // mouse X between coll.x and coll.x + coll.w
    if (event.button.x >= coll.x && event.button.x <= (coll.x + coll.w)) {
        // mouse Y between coll.y and coll.y + coll.h
        if (event.button.y >= coll.y && event.button.y <= (coll.y + coll.h)) {
            // clicked on "back layer" box or "front layer"
            if (coll.y == iLyY[0])
                Flags.set(FLAGS_CHK_LAYER);
            else
                Flags.unset(FLAGS_CHK_LAYER);
        }
    }

    // mouse inside the left screen
    else if (event.button.x < WIDTH)
        return _mouse_over_left_screen(event);

return 0;
}

static int _keyboard(SDL_Event& event) {
    using level::Flags;

    // return value
    int8_t i8Return = 0;

    switch (event.key.keysym.sym) {
    default:
        break;

    case SDLK_f:
        {
            // don`t activate line-fill if text input mode is enabled
            if (Flags.mask(FLAGS_TEXT_INPUT)) break;
            
            // break if tile selection mode is enabled
            else if (Flags.mask(FLAGS_TILE_SEL)) break;
            
            // if on tile A or B
            if (Flags.mask(FLAGS_FILL_BOTH)) {
                Flags.unset(FLAGS_FILL_BOTH);
                Flags.set(FLAGS_FILL_NULL);
            }
            
            // activate line fill mode; disable camera
            else {
                Flags.set(FLAGS_FILL_ST);
                Flags.unset(FLAGS_FILL_NULL | FLAGS_CAMERA);
            }
            break;
        }

    case SDLK_t:
        {
            // if text input is enabled, don`t allow this
            if (Flags.mask(FLAGS_TEXT_INPUT)) break;
            
            // enable tile selection; disable line fill and camera
            if (! Flags.mask(FLAGS_TILE_SEL)) {
                Flags.set(FLAGS_TILE_SEL | FLAGS_FILL_NULL);
                Flags.unset(FLAGS_FILL_ST | FLAGS_FILL_END | FLAGS_CAMERA);
            }
            else
                Flags.unset(FLAGS_TILE_SEL);
        }
        break;

    case SDLK_c:
        {
            // do not scroll the camera if text input is enabled
            if (Flags.mask(FLAGS_TEXT_INPUT)) break;
            
            // can't scroll the camera if "tile selection" mode is enabled
            else if (Flags.mask(FLAGS_TILE_SEL)) break;

            // enabling camera scroll disables "fill line" mode automatically
            if (! Flags.mask(FLAGS_CAMERA)) {
                Flags.set(FLAGS_CAMERA | FLAGS_FILL_NULL);
                Flags.unset(FLAGS_FILL_BOTH);
            }
            else {Flags.unset(FLAGS_CAMERA);}
        }
        break;

    case SDLK_s:
        // if text input is enabled, don`t allow this
        if ( Flags.mask(FLAGS_TEXT_INPUT) ) break;
        
        // turn on sprite mode
        if (! Flags.mask(FLAGS_SPRITE_SEL)) {
            Flags.set(FLAGS_FILL_NULL|FLAGS_SPRITE_SEL);
            Flags.unset(FLAGS_FILL_BOTH|FLAGS_TILE_SEL);
        }
        
        // turn off sprite mode
        else {
            Flags.unset(FLAGS_SPRITE_SEL);
        }
        break;

    case SDLK_RIGHT:
        if (Flags.mask(FLAGS_CAMERA)) {
            keymasks = 1;
        }
        break;

    case SDLK_LEFT:
        if (Flags.mask(FLAGS_CAMERA)) {
            keymasks = 2;
        }
        break;

    case SDLK_RETURN:
        if (! Flags.mask(FLAGS_TEXT_INPUT) && Flags.mask(FLAGS_SPRITE_SEL)) {
            startTextInput();
            Flags.unset(FLAGS_FILL_BOTH|FLAGS_TILE_SEL|FLAGS_CAMERA);
        }
        else {
            Flags.unset(FLAGS_TEXT_INPUT);
        }
        break;
    
    case SDLK_BACKSPACE:
        if ( Flags.mask(FLAGS_TEXT_INPUT) ) {
            popChar();
        }
        break;
    
    case SDLK_ESCAPE:
        i8Return = 1;
        break;
    }

return i8Return;
}

int eventMain(SDL_Event* event) {
    int8_t i8Return = 0;

    while (SDL_PollEvent(event)) {
        switch (event->type) {
        default:
            break;

        case SDL_KEYDOWN:
            i8Return = _keyboard(*event);
            break;
        
        case SDL_TEXTINPUT:
            addText(event->text.text);
            break;
        
        case SDL_KEYUP:
            keymasks = 0;
            break;

        case SDL_MOUSEBUTTONDOWN:
            i8Return = _mousedown(*event);
            break;

        case SDL_QUIT:
            i8Return = 1;
            break;
        }
    }

    // holding right
    if (keymasks == 1)
        camera::scrollx(TILE_WIDTH/2);

    // holding left
    else if (keymasks == 2)
        camera::scrollx(-TILE_WIDTH/2);

return i8Return;
}
