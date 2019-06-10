#include "event.h"
#include "level.hpp"
#include "camera.h"
#include "graphics.h"

static uint8_t keymasks = 0;

static int _keyboard(SDL_Event& event);
static int _mousedown(SDL_Event& event);
//static int _keyrelease(SDL_Event& event);

int eventMain(SDL_Event& event) {
    int8_t i8Return = 0;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        default:
            break;

        case SDL_KEYDOWN:
            i8Return = _keyboard(event);
            break;

        case SDL_KEYUP:
            keymasks = 0;
            break;

        case SDL_MOUSEBUTTONDOWN:
            i8Return = _mousedown(event);
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

int _mouse_over_left_screen(SDL_Event& event) {
    /* Tile at (x,y) where mouse was clicked. X coordinate is offset
       by the X coordinate of the camera. */
    using level::header;
    uint16_t desttile = gettile(header->width, event.button.x + camera::cam.x,
                                event.button.y);
    using level::flags;

    // tile selection mode
    if (flags.mask(FLAGS_TILE_SEL)) {
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
                    using level::tilenums;
                    tilenums[0] = gettile(16, mouse.x(), mouse.y());
                    printTile();
                    break;
                }

            case SDL_BUTTON_RIGHT:
                {
                    using level::tilenums;
                    tilenums[1] = gettile(16, mouse.x(), mouse.y());
                    printTile();
                    break;
                }
            }

        } // end if -- mouse within bounds
    } // end if -- tile selection mode

    // fill-line mode enabled
    else if (flags.mask(FLAGS_FILL_BOTH)) {
        // which stage of fill-line mode are we in?
        char cFirstLast = (char) flags.get() & FLAGS_FILL_BOTH;

        // starting phase
        if (cFirstLast == FLAGS_FILL_ST) {
            using level::tilenums;

            // mark the first tile in a straight line; initiate the next stage
            tilenums[2] = desttile;
            flags.unset(FLAGS_FILL_ST);
            flags.set(FLAGS_FILL_END);
        }

        // final phase
        else {
            using level::tilenums;

            // mark the last tile in the line; turn off fill-line mode
            tilenums[3] = desttile;
            flags.unset(FLAGS_FILL_END);
            flags.set(FLAGS_FILL_NULL);

            // fill each tile in the line with the selected tile number
            level::changeLine(level::bg->renderer);
        }
    }

    // paste a single tile onto the level
    else {
        // right-click a tile
        if (event.button.button == SDL_BUTTON_RIGHT) {
            using level::tilenums;
            tilenums[0] = level::vtiles[desttile].graphicID;
            tilenums[1] = level::vtiles[desttile].codeID;
            printTile();
        }
        // left click a tile
        else
            level::changeTile(level::bg->renderer, desttile);
    }

return 0;
}

int _mousedown(SDL_Event& event) {
    using level::flags;

    // index to iLyY; which box to check
    uint8_t shLayer = flags.mask(FLAGS_CHK_LAYER);

    // Y offset for coll: box to draw check mark on
    int iLyY[] = {144, 120};

    // collision rectangle of layer check box
    SDL_Rect coll = {7 + WIDTH, 0, 15, 15};
    coll.y = iLyY[shLayer];

    // mouse X between coll.x and coll.x + coll.w
    if (event.button.x >= coll.x && event.button.x <= (coll.x + coll.w)) {
        // mouse Y between coll.y and coll.y + coll.h
        if (event.button.y >= coll.y && event.button.y <= (coll.y + coll.h)) {
            // clicked on "back layer" box or "front layer"
            if (coll.y == iLyY[0])
                flags.set(FLAGS_CHK_LAYER);
            else
                flags.unset(FLAGS_CHK_LAYER);
        }
    }

    // mouse inside the left screen
    else if (event.button.x < WIDTH)
        return _mouse_over_left_screen(event);

return 0;
}

int _keyboard(SDL_Event& event) {
    using level::flags;

    // return value
    int8_t i8Return = 0;

    switch (event.key.keysym.sym) {
    default:
        break;

    case SDLK_f:
        {
            // if in tile selection mode, don't allow this
            if (flags.mask(FLAGS_TILE_SEL)) break;

            if (flags.mask(FLAGS_FILL_BOTH)) {
                flags.unset(FLAGS_FILL_BOTH);
                flags.set(FLAGS_FILL_NULL);
            }
            else {
                flags.set(FLAGS_FILL_ST);
                flags.unset(FLAGS_FILL_NULL | FLAGS_CAMERA);
            }
            break;
        }

    case SDLK_t:
        {
            if (! flags.mask(FLAGS_TILE_SEL)) {
                flags.set(FLAGS_TILE_SEL | FLAGS_FILL_NULL);
                flags.unset(FLAGS_FILL_ST | FLAGS_FILL_END | FLAGS_CAMERA);
            }
            else
                flags.unset(FLAGS_TILE_SEL);
        }
        break;

    case SDLK_c:
        {
            // can't scroll the camera if "tile selection" mode is enabled
            if (flags.mask(FLAGS_TILE_SEL)) break;

            // enabling camera scroll disables "fill line" mode automatically
            if (! flags.mask(FLAGS_CAMERA)) {
                flags.set(FLAGS_CAMERA | FLAGS_FILL_NULL);
                flags.unset(FLAGS_FILL_BOTH);
            }
            else {flags.unset(FLAGS_CAMERA);}
        }
        break;

    case SDLK_RIGHT:
        if (flags.mask(FLAGS_CAMERA)) {
            keymasks = 1;
        }
        break;

    case SDLK_LEFT:
        if (flags.mask(FLAGS_CAMERA)) {
            keymasks = 2;
        }
        break;

    case SDLK_ESCAPE:
        i8Return = 1;
        break;
    }

return i8Return;
}
