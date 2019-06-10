#include "graphics.h"
#include "level.hpp"

// namespace graphics
std::vector<Image>  graphics::gui;
int                 graphics::bgx;

using std::cerr;
using namespace graphics;

void updateLSCRN() {
    using level::layers;
    using level::bg;
    using camera::cam;

    // renderer
    SDL_Renderer* ren = bg->renderer;

    // bg
    bg->blit(bgx, 0, &cam);
    bg->blit(bgx + WIDTH, 0, &cam);

    // if camera to the left of bg
    if (cam.x < bgx) {
        bgx -= WIDTH;
    }

    // if camera to the right of bg
    else if ((cam.x + cam.w) > (bgx + WIDTH * 2)) {
        bgx += WIDTH;
    }

    // layer 0, shader, layer 1
    SDL_RenderCopy(ren, layers[0], &cam, nullptr);
    SDL_RenderCopy(ren, layers[Shader], nullptr, nullptr);
    SDL_RenderCopy(ren, layers[1], &cam, nullptr);

    // if tile select is activated
    using level::flags;
    if (flags.mask(FLAGS_TILE_SEL)) {
        using level::tileset;
        SDL_Rect dstrect = {192, 112, 256, 256};

        SDL_RenderCopy(ren, layers[Shader], nullptr, &dstrect);
        SDL_RenderCopy(ren, tileset, nullptr, &dstrect);
    }

    // if the camera is enabled
    else if (flags.mask(FLAGS_CAMERA)) {
        // render arrow graphics
        gui[2].blit(576, 416);
    }
}

void updateRSCRN() {
    using level::flags;

    static const int _data_chk_y[4] = {
        121, 145,
        276, 300
    };

    // sidebar image
    gui[GUI_BACKGROUND].blit();

    // blit a tile at (43,83)
    {
        auto itrTile = level::tilenums.begin();

        // put source tile at (43,83) on the right screen
        Point<int> temp = getXY(256, *itrTile);
        SDL_Rect src = {temp.x(), temp.y(), TILE_WIDTH, TILE_HEIGHT};
        SDL_Rect dst = {43, 83, 15, 15};

        SDL_RenderCopy(gui[0].renderer, level::tileset, &src, &dst);
    }

    // checkmark image
    {
        int idx = (int) flags.mask(FLAGS_CHK_LAYER);
        gui[GUI_CHK].blit(8, _data_chk_y[idx]);
    }

    // render check mark if line-fill is activated
    if (! (flags.get() & FLAGS_FILL_NULL)) {
        int idx = (flags.get() & FLAGS_FILL_BOTH) / FLAGS_FILL_ST - 1;
        gui[GUI_CHK].blit(9, _data_chk_y[idx+2]);
    }
}

bool loadGlobalMedia(SDL_Renderer* ren) {
    // make vector
    gui.resize(3);

    // load checkmark image
    auto itr = gui.begin();
    itr->renderer = ren;
    itr->open("chk.png");
    if (itr->error()) {
        cerr << itr->get_error() << '\n';
        return false;
    }

    // load bg image
    ++itr;
    itr->renderer = ren;
    itr->open("guibar.jpg");
    if (itr->error()) {
        cerr << itr->get_error() << '\n';
        return false;
    }

    // load arrow images
    ++itr;
    itr->renderer = ren;
    if (! itr->open("arrows.png")) {
        cerr << itr->get_error() << '\n';
        return false;
    }

return true;
}
