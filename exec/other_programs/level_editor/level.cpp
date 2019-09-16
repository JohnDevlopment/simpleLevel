#include "level.hpp"
#include "graphics.h"
#include <SDL.h>
#include <memory>

Bitfield<uint32_t> level::Flags(FLAGS_FILL_NULL);
generic_class<Image> level::Background;
SDL_Texture* level::Tileset = nullptr;
SDL_Texture* level::Layers[4] = {nullptr, nullptr, nullptr, nullptr};
StaticDArray<unsigned int, 4> level::TileNumbers;
generic_class<LevelHeader> level::Header;
Tile* level::Tiles;
std::vector<SprObj> level::Sprites;
unsigned int level::SpriteID = 0;

// private functions
static bool _newLevel(SDL_Renderer* ren);
static bool _newShader(SDL_Renderer* ren);
static bool _newBackground(const uint8_t id);
static bool _newTileset(const uint8_t id, SDL_Renderer* ren);
static void _buildTilemap(SDL_Renderer* ren, const SprObj* sprites);
static std::vector<SprObj>::iterator searchSpriteVector(int x, int y);

using namespace level;
using std::cerr;
using std::endl;

constexpr uint8_t _data_tbl1[16] = {
    0,   16,  32,  48,  64,
    80,  96,  112, 128, 144,
    160, 176, 192, 208, 224,
    240
};

// these functions are outside the namespace but implemented here all the same
void printTile() {
    std::cout << "Graphic Tile:\t" << TileNumbers[0] << "\nCode Tile:\t" \
    << TileNumbers[1] << endl;
}

uint16_t gettile(int roww, int x, int y) {

    // convert XY coordinate from pixel to tile
    x = x != 0 ? x / TILE_WIDTH  : 0;
    y = y != 0 ? y / TILE_HEIGHT : 0;

    // for every Y, add a row's worth of tiles to X
    while (y > 0) {
        x += roww;
        x += roww;
        --y;
        --y;
    }

    // if Y < 0, offset X to rebalance it
    if (y < 0) {
        x -= roww;
        ++y;
    }

return static_cast<uint16_t>(x);
}

Point<int> getXY(int tbw, uint16_t tile) {
    Point<int> retval;

    int x     = TILE_WIDTH * tile;
    retval[0] = x - tbw * (x / tbw);
    retval[1] = x / tbw * TILE_HEIGHT;

return std::move(retval);
}

// namespace functions
void changeLine(SDL_Renderer* ren) {
    using std::cout;

    // width of the level in pixels
    const int imgw = Header->width * TILE_WIDTH;

    // current tile
    Point<int> curtile;

    // line between two points
    PVector<int> line;
    
    {
        curtile = getXY(imgw, TileNumbers[2]);
        Point<int> endtile = getXY(imgw, TileNumbers[3]);
        
        // difference between two points
        line = endtile - curtile;
        if (line == 0) {
            cerr << "changeLine(): the same tile was selected twice\n";
            return;
        }

        // simplify the vector
        line.transform(simp);
    }
    
    // if one is zero, change the other to 1
    if (! line.x())
        line[1] = line.y() / tabs(line.y());
    else
        line[0] = line.x() / tabs(line.x());
    
    // only allow lines where one coordinate is 0
    {
        PVector<int> compVal(1, 0);

        // invalid if both fields are non-zero
        if (line.x() != 0 && line.y() != 0) return;
        
        // if line isn't (1,0) or (0,1), return
        if (line != compVal && line != compVal.swap()) return;
    }
    
    // multiply the line by tile size to decide the step of each tile
    line[0] = line.x() * TILE_WIDTH;
    line[1] = line.y() * TILE_HEIGHT;
    
    // current TO tile
    uint16_t desttile = TileNumbers[2];

    // change the first tile in the range
    changeTile(ren, desttile);

    while (desttile != TileNumbers[3]) {
        curtile += line;
        desttile = gettile(Header->width, curtile.x(), curtile.y());
        changeTile(ren, desttile);
    }
}

void changeSpriteTile(SDL_Renderer* ren, uint16_t dtile, const bool del) {
    Tile& tile = Tiles[dtile];
    SDL_Rect destination = {0, 0, TILE_WIDTH, TILE_HEIGHT};
    SprObj mySprite;
    
    // initialize sprite values
    mySprite.id = SpriteID;
    mySprite.flags = 0;
    
    // get XY position of the destination tile
    if (dtile) {
        Point<int> tileLoc = getXY(Header->width * TILE_WIDTH, dtile);
        destination.x = tileLoc.x();
        destination.y = tileLoc.y();
        
        mySprite.x = tileLoc.x() / TILE_WIDTH;
        mySprite.y = tileLoc.y() / TILE_HEIGHT;
    }
    
    // set the sprite layer as the current rendering target
    SDL_SetRenderTarget(ren, Layers[SpriteTilemap]);
    
    // simply delete the tile
    if (del) {
        auto itr = searchSpriteVector(mySprite.x, mySprite.y);
        
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
        SDL_RenderFillRect(ren, &destination);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 0xff);
        
        // delete specified element in vector
        if (itr != Sprites.end()) {
            itr = Sprites.erase(itr);
            Header->sprites--;
        }
    }
    else {
        SDL_RenderCopy(ren, SpriteTile, nullptr, &destination);
        Sprites.push_back(mySprite);
        Header->sprites++;
    }
    
    // revert to rendering to screen
    SDL_SetRenderTarget(ren, nullptr);
}

void changeTile(SDL_Renderer* ren, uint16_t dtile) {
    StaticDArray<SDL_Rect,2> srcdst;
    Tile& tile = Tiles[dtile];
    const short int iOldLayer = WhichLayer(tile.flags);
    const short int iNewLayer = Flag_Layer( Flags.get() );

    for (int x = 0; x < 2; ++x) {
        srcdst[x].w = TILE_WIDTH;
        srcdst[x].h = TILE_HEIGHT;
    }

    // get XY positions of each tile
    {
        Point<int> txy = getXY(256, TileNumbers[0]);
        srcdst[0].x = txy.x();
        srcdst[0].y = txy.y();

        txy = getXY(Header->width * TILE_WIDTH, dtile);
        srcdst[1].x = txy.x();
        srcdst[1].y = txy.y();
    }

    // set draw color as transparent
    // clear previous tile
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
    SDL_SetRenderTarget(ren, Layers[iOldLayer]);
    SDL_RenderFillRect(ren, &srcdst[1]);

    /* If new tile graphic is to be on different layer from the old, tile
       layer flag is flipped and other layer is set at render target. */
    if (iOldLayer != iNewLayer) {
        SDL_SetRenderTarget(ren, Layers[iNewLayer]);
        tile.flags ^= TILEFLAG_LAYER;
    }

    // change the tile's graphic id and code id according to the selected values
    tile.graphicID = TileNumbers[0];
    tile.codeID    = TileNumbers[1];

    // draw tile if graphic id isn't zero
    if (tile.graphicID)
        SDL_RenderCopy(ren, Tileset, &srcdst[0], &srcdst[1]);

    // set render target to the whole screen
    // set render draw color to solid black
    SDL_SetRenderTarget(ren, nullptr);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0xff);
}

bool level::build(const char* file, const Program& pg) {
    using namespace std;
    
    int retval;
    SprObj* fileSpriteArray = nullptr;
    
    // read file
    retval = Level_ReadFile(file, Header.getp(), &Tiles, &fileSpriteArray);
    if (retval < 0) {
        cerr << "Level_ReadFile: " << Level_GetError() << '\n';
        return false;
    }
    
    // copy sprites into SprObj vector
    if (fileSpriteArray) {
//        SprObj* next_sprite = fileSpriteArray;
//        
//        cout << "Number of sprites: " << Header->sprites << endl;
//        Sprites.resize(static_cast<int>(Header->sprites));
//        for (auto&& itr : Sprites) {
//            itr = *next_sprite++;
//        }
        cout << "Number of sprites defined in array: " << Sprites.size() << endl;
//        delete[] fileSpriteArray;
    }
    
    // define camera dimensions
    {
        SDL_Rect temp = {0, 0, Header->width * TILE_WIDTH, Header->height * TILE_HEIGHT};
        camera::define(temp);
    }
    
    // create two textures to act as the tilemap layers
    if (! _newLevel(pg.renderer))
        return false;
    
    // create texture that "shades" between the two layers
    if (! _newShader(pg.renderer))
        return false;
    
    // load the background image
    Background->renderer = pg.renderer;
    if (! _newBackground(Header->background))
        return false;
    
    // load the tileset
    if (! _newTileset(Header->tileset, pg.renderer))
        return false;
    
    // build the tilemap based on the vector array
    _buildTilemap(pg.renderer, fileSpriteArray);
    
    // free memory used in this function
    if (fileSpriteArray) {
        delete[] fileSpriteArray;
        fileSpriteArray = nullptr;
    }
    
    // print information about the level
    printf("Level:\twidth\t= %d tiles\n\t\theight\t= %d tiles\n\t\tbg\t= %d\n",
    	static_cast<int>(Header->width), static_cast<int>(Header->height),
    	static_cast<int>(Header->background));
    
    cout << "\t\ttileset = " << static_cast<int>(Header->tileset) << endl;
    
    printf("\t\tmusic\t= %d\n\t\tsprites\t= %d count\n", static_cast<int>(Header->music),
    	static_cast<int>(Header->sprites));
    
return true;
}

// Private Functions //
void _buildTilemap(SDL_Renderer* ren, const SprObj* sprites) {
    SDL_Rect src = {0, 0, TILE_WIDTH, TILE_HEIGHT};
    SDL_Rect dst = {0, 0, TILE_WIDTH, TILE_HEIGHT};
    const int imgw = Header->width * TILE_WIDTH;
    const int numtiles = Header->width * Header->height;
    
    for (uint16_t a = 0; a < numtiles; ++a) {
        Tile* tileA = &Tiles[a];
        Point<int> p;

        // skip empty tiles
        SDL_SetRenderTarget(ren, Layers[WhichLayer(tileA->flags)]);
        if (! tileA->graphicID)
            continue;

        // get the location of the source tile
        p = getXY(256, tileA->graphicID);
        src.x = p.x();
        src.y = p.y();
        
        // get the destination location
        p = getXY(imgw, a);
        dst.x = p.x();
        dst.y = p.y();
        
        // copy tile to tilemap
        SDL_RenderCopy(ren, Tileset, &src, &dst);
    }
    
    // add sprites
    if (sprites) {
        uint16_t uiTile;
        const int iSprites = static_cast<int>(Header->sprites);
        Header->sprites = 0;
        
        for (int x = 0; x < iSprites; ++x) {
            // location of sprite tile
            uiTile = gettile_inline(Header->width, sprites[x].x, sprites[x].y);
            
            // update sprite tile to be the same as in the file
            changeSpriteTile(ren, uiTile, false);
            
            // print location of sprite
            std::printf("Add sprite to tile %u (located at <%d, %d>)\n",
                uiTile, static_cast<int>(sprites[x].x), static_cast<int>(sprites[x].y));
        }
    }
    
    // do not render to a texture anymore
    SDL_SetRenderTarget(ren, nullptr);
}

bool _newTileset(const uint8_t id, SDL_Renderer* ren) {
    SDL_Surface* tsuf = nullptr;
    std::string file = "images/tilesets/";
    bool retval = true;

    // file based on integer argument
//    switch (id) {
//    default:
//    case 0:
//        file += "000.png";
//        break;

//    case 1:
//        file += "001.png";
//        break;
//    }

    if (id == 1) {
        file += "001.png";
    }
    else {
        file += "000.png";
    }

    // load surface
    tsuf = IMG_Load(file.c_str());
    file = IMG_GetError();
    retval = tsuf != nullptr;

    if (retval) {
        // convert surface to texture
        Tileset = SDL_CreateTextureFromSurface(ren, tsuf);
        file = SDL_GetError();
        retval = Tileset != nullptr;

        // free the surface and make it not point to anything
        SDL_FreeSurface(tsuf);
        tsuf = nullptr;
    }
    
    // print any errors
    if (! retval) {
        cerr << "_newTileset: " << file << '\n';
    }

return retval;
}

bool _newBackground(const uint8_t id) {
    // image file chosen based on id number
    switch (id) {
    case 0:
        Background->open("images/backgrounds/clear_sky.png");
        break;

    case 1:
        Background->open("images/backgrounds/grassy_field.png");
        break;

    default:
        cerr << "_newBackground: invalid id number " << (int) id << '\n';
        return false;
        break;
    }

    // process error if it happens
    if (Background->error()) {
        cerr << "_newBackground: " << Background->get_error() << '\n';
        return false;
    }

    // configure bg
    Background->set_blit_size(WIDTH, HEIGHT);

return true;
}

bool _newShader(SDL_Renderer* ren) {
    /* [pattern] is a table of values that act as yes or no flags. To wit,
       if a given value is not zero, a pixel should be drawn in; otherwise
       it should be transparent. To index this table, you need an index first
       to the start of each "row" -- that is, each group of four -- and then
       add 0 - 4 to that index. For example, if the current pixel is at
       (30, 15), pttidx equals 8 : (30 MOD 4) * 4. Then you add 3 to 8 because
       15 MOD 4 = 3; thus we get 11. */
//    constexpr uint8_t pattern[16] = {
//        1, 0, 0, 0,
//        0, 1, 0, 0,
//        0, 0, 1, 0,
//        0, 0, 0, 1
//    };
    
    constexpr uint8_t _pattern[16] = {
        1, 0, 0, 1,
        0, 1, 1, 0,
        0, 1, 1, 0,
        1, 0, 0, 1
    };
    
    SDL_Surface* tempSurface;
    std::string error;
    
    // create new 16-bit surface
    tempSurface = SDL_CreateRGBSurfaceWithFormat(0, WIDTH, HEIGHT, 16, SDL_PIXELFORMAT_RGBA4444);
    if (tempSurface == nullptr) {
        cerr << "_newShader: " << SDL_GetError() << '\n';
        return false;
    }
    
    // map 16-bit color value
    //const uint32_t uiColor = SDL_MapRGBA(tempSurface->format, 0x55, 0x55, 0x55, 0xff);
    const uint32_t uiColor = SDL_MapRGB(tempSurface->format, 0x55, 0x55, 0x55);
    
    // array of pixels
    uint16_t* uipPixels = (uint16_t*) tempSurface->pixels;
    
    /* Go down each row and change its pixels. [pttidx] is an index to
       the table at [_pattern]. [pixel] is an index to an array
       representing the surface's pixel map. */
    for (int y = 0; y < tempSurface->h; ++y) {
        uint8_t uiIndex = (y & 3) * 4;
        int iPixel = tempSurface->w * y;

        // each pixel on a row
        for (int x = 0; x < tempSurface->w; ++x) {
            if (_pattern[uiIndex + x % 4])
                uipPixels[iPixel] = uiColor;

            ++iPixel;
        }
    }

    // convert surface to texture
    Layers[Shader] = SDL_CreateTextureFromSurface(ren, tempSurface);
    error = SDL_GetError();

    // free memory used by the surface
    SDL_FreeSurface(tempSurface);

    // return an error if the texture is null
    if (Layers[Shader] == nullptr) {
        cerr << "SDL_CreateTextureFromSurface: " << error << '\n';
        return false;
    }

return true;
}

bool _newLevel(SDL_Renderer* ren) {
    // back and frontal layer are [0] and [1] respectively
    for (int x = 0; x < 2; ++x) {
        // create texture that can be rendered to
        Layers[x] = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA4444,
                                      SDL_TEXTUREACCESS_TARGET, Header->width * TILE_WIDTH,
                                      Header->height * TILE_HEIGHT);
        if (Layers[x] == nullptr) {
            cerr << "SDL_CreateTexture: " << SDL_GetError() << '\n';
            return false;
        }

        // enable transparency
        SDL_SetTextureBlendMode(Layers[x], SDL_BLENDMODE_BLEND);
    }

    // sprite tilemap, 8-bit indexed
    Layers[SpriteTilemap] = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA4444, SDL_TEXTUREACCESS_TARGET,
        Header->width * TILE_WIDTH, Header->height * TILE_HEIGHT);
    if (Layers[SpriteTilemap] == nullptr) {
        cerr << "SDL_CreateTexture: " << SDL_GetError() << '\n';
        return false;
    }
    SDL_SetTextureBlendMode(Layers[SpriteTilemap], SDL_BLENDMODE_BLEND);

return true;
}

std::vector<SprObj>::iterator searchSpriteVector(int x, int y) {
    std::vector<SprObj>::iterator itr;
    
    // search vector using x and y position
    itr = Sprites.begin();
    while (itr != Sprites.end()) {
        if (itr->x == x && itr->y == y)
            break;
        ++itr;
    }
    
return itr;
}
