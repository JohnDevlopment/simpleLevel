#include "graphics.h"
#include "level.hpp"

#define SEGCHECK()	std::cout << __FILE__ << ", line " << __LINE__ << ": checking for segmentation fault\n";
#define SEGEND()	std::cout << __FILE__ << ", line " << __LINE__ << ": no segmentation fault here\n";

Image* GUI = nullptr;
int BGX = 0;
SDL_Texture* SpriteTile = nullptr;

using std::cerr;

void updateLSCRN() {
    using namespace level;
    using camera::Cam;
    
    // renderer
    SDL_Renderer* ren = Background->renderer;

    // bg
    Background->blit(BGX, 0, &Cam);
    Background->blit(BGX + WIDTH, 0, &Cam);

    // if camera to the left of bg
    if (Cam.x < BGX) {
        BGX -= WIDTH;
    }
    
    // if camera to the right of bg
    else if ((Cam.x + Cam.w) > (BGX + WIDTH * 2)) {
        BGX += WIDTH;
    }

    // layer 0, shader, layer 1
    SDL_RenderCopy(ren, Layers[0], &Cam, nullptr);
    SDL_RenderCopy(ren, Layers[Shader], nullptr, nullptr);
    SDL_RenderCopy(ren, Layers[1], &Cam, nullptr);
    
    
    {
        int iFlags = Flags.get();
        
        if (iFlags & FLAGS_TILE_SEL) {
            SDL_Rect dstrect = {192, 112, 256, 256};
            SDL_RenderCopy(ren, Layers[Shader], nullptr, &dstrect);
            SDL_RenderCopy(ren, Tileset, nullptr, &dstrect);
        }
        
        else if (iFlags & FLAGS_SPRITE_SEL) {
            SDL_RenderCopy(ren, Layers[SpriteTilemap], &Cam, nullptr);
            
            if (iFlags & FLAGS_TEXT_INPUT) {
                // render text input message
                GUI[GUI_MSG_OF_TI].blit(50, 50);
                if (GUI[GUI_MSG_OF_TI].error()) {
                    cerr << GUI[GUI_MSG_OF_TI].get_error() << '\n';
                    Flags.unset(FLAGS_TEXT_INPUT);
                }
            }
        }
        
        else if (iFlags & FLAGS_CAMERA) {
            // render arrow graphics
            GUI[2].blit(576, 416);
        }
    }
}

void updateRSCRN() {
    using namespace level;
    
    static const int _data_chk_y[4] = {
        121, 145,
        276, 300
    };
    
    // sidebar image
    GUI[GUI_BACKGROUND].blit();
    
    // blit a tile at (43,83)
    {
        SDL_Rect src = {0, 0, TILE_WIDTH, TILE_HEIGHT};
        SDL_Rect dst = {43, 83, 15, 15};
        SDL_Texture* tex;
        const SDL_Rect* p_src;
        
        // sprite selection mode
        if (Flags.mask(FLAGS_SPRITE_SEL)) {
            p_src = nullptr;
            tex = SpriteTile;
        }
        else {
            Point<int> srcxy = getXY(256, TileNumbers[0]);
            src.x = srcxy.x();
            src.y = srcxy.y();
            
            p_src = &src;
            tex = Tileset;
        }
        
        SDL_RenderCopy(GUI->renderer, tex, p_src, &dst);
    }

    // checkmark image
    {
        int idx = (int) Flags.mask(FLAGS_CHK_LAYER);
        GUI[GUI_CHK].blit(8, _data_chk_y[idx]);
    }

    // render check mark if line-fill is activated
    if (! (Flags.get() & FLAGS_FILL_NULL)) {
        int idx = (Flags.get() & FLAGS_FILL_BOTH) / FLAGS_FILL_ST - 1;
        GUI[GUI_CHK].blit(9, _data_chk_y[idx+2]);
    }
}

static void free() {
    delete[] GUI;
    SDL_DestroyTexture(SpriteTile);
    BGX = 0;
}

SDL_Texture* loadTex(SDL_Renderer* ren, const char* file) {
    SDL_Texture* texture = nullptr;
    SDL_Surface* surface = IMG_Load(file);
    
    if (surface == nullptr) {
        cerr << "failed to load " << file << " : " << SDL_GetError() << '\n';
    }
    else {
        texture = SDL_CreateTextureFromSurface(ren, surface);
        SDL_FreeSurface(surface);
        if (texture == nullptr) {
            cerr << "failed to convert surface (from " << file << ") : " \
            << SDL_GetError() << '\n';
        }
    }
    
return texture;
}

bool loadGlobalMedia(SDL_Renderer* ren) {
    GUI = new Image[4];
    std::atexit(free);
    
    // sprite tile image
    SpriteTile = loadTex(ren, "sprite.png");
    if (SpriteTile == nullptr)
        return false;
    
    // load checkmark image
    auto itr = GUI;
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
    
    // text saying that text input is enabled
    ++itr;
    itr->renderer = ren;
    if (! itr->open("ti_msg.png")) {
        cerr << itr->get_error() << '\n';
        return false;
    }
    
return true;
}
