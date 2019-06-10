#include "level.hpp"

#include <SDL.h>

generic_class<LevelHeader>  level::header;
std::vector<Tile>           level::vtiles;
SDL_Texture*                level::layers[3];
generic_class<Image>        level::bg;
SDL_Texture*                level::tileset = nullptr;
Bitfield<uint32_t>          level::flags(FLAGS_FILL_NULL);
std::vector<unsigned int>   level::tilenums(4);

// private functions
static bool _newLevel(SDL_Renderer* ren);
static bool _newShader(SDL_Renderer* ren);
static bool _newBackground(const uint8_t id);
static bool _newTileset(const uint8_t id, SDL_Renderer* ren);
static void _buildTilemap(SDL_Renderer* ren);
//static uint16_t incrTile(uint16_t tile, const PVector<int>& incs) __attribute__ ((pure));
//static PVector<int> PVmod(int& x, int& y);

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
    using level::tilenums;

    std::cout << "Graphic Tile:\t" << tilenums[0] << "\nCode Tile:\t" \
    << tilenums[1] << endl;
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
void level::changeLine(SDL_Renderer* ren) {
    using std::cout;
    using std::endl;

    // width of the level in pixels
    const int imgw = header->width * TILE_WIDTH;

    // current tile
    Point<int> curtile;

    // line between two points
    PVector<int> line;
    {
        Point<int> tile1 = getXY(imgw, tilenums[2]);
        Point<int> tile2 = getXY(imgw, tilenums[3]);

        // current tile being modified
        curtile = tile1;

        // difference between two points
        line = tile2 - tile1;

        // if the same tile is selected twice, the difference will be 0
        if (line == 0) {
            std::cerr << "changeLine(): the same tile was selected twice\n";
            return;
        }

        // simplify the vector
        line.transform(simp);
    }

    // if one is zero, change the other to 1
    if (! line.x())
        line[1] = line.y() / tabs(line.y());
    else if (! line.y())
        line[0] = line.x() / tabs(line.x());

    // only allow lines where one coordinate is 0
    {
        PVector<int> compVal(1, 0);

        // invalid if both fields are non-zero
        if (line.x() != 0 && line.y() != 0) return;

        // if line isn't (1,0) or (0,1), return
        if (line != compVal && line != compVal.swap()) return;
    }

    // DEBUG
    cout << "line = " << line << endl;

    // multiply the line by tile size to decide the step of each tile
    line = line * line.create(TILE_WIDTH, TILE_HEIGHT);

    // DEBUG
    cout << "line = " << line << endl;

    // current TO tile
    uint16_t desttile = tilenums[2];

    // change the first tile in the range
    changeTile(ren, desttile);

    while (desttile != tilenums[3]) {
        curtile += line;
        desttile = gettile(header->width, curtile.x(), curtile.y());
        changeTile(ren, desttile);
    }
}

void level::changeTile(SDL_Renderer* ren, uint16_t dtile) {
    StaticDArray<SDL_Rect,2> srcdst;

    // source & destination rectangles
    {
        SDL_Rect temp = {0, 0, TILE_WIDTH, TILE_HEIGHT};
        srcdst[0] = temp;
        srcdst[1] = temp;
    }

    // get XY positions of each tile
    {
        Point<int> txy = getXY(256, tilenums[0]);
        srcdst[0].x = txy.x();
        srcdst[0].y = txy.y();

        txy = getXY(header->width * TILE_WIDTH, dtile);
        srcdst[1].x = txy.x();
        srcdst[1].y = txy.y();
    }

    // reference to the tile
    Tile& tile = vtiles[dtile];

    // layer of old tile
    const short layer_old = WhichLayer(tile.flags);

    // layer of new tile
    const short layer_new = Flag_Layer(flags.get());

    // set draw color as transparent
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);

    // clear previous tile
    SDL_SetRenderTarget(ren, layers[layer_old]);
    SDL_RenderFillRect(ren, &srcdst[1]);

    /* If new tile graphic is to be on different layer from the old, tile
       layer flag is flipped and other layer is set at render target. */
    if (layer_old != layer_new) {
        SDL_SetRenderTarget(ren, layers[layer_new]);
        tile.flags ^= TILEFLAG_LAYER;
    }

    // change the tile's graphic id and code id according to the selected values
    tile.graphicID = tilenums[0];
    tile.codeID    = tilenums[1];

    // draw tile if graphic id isn't zero
    if (tile.graphicID)
        SDL_RenderCopy(ren, tileset, &srcdst[0], &srcdst[1]);

    // set render target to the whole screen
    SDL_SetRenderTarget(ren, nullptr);

    // set render draw color to solid black
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0xff);
}

int level::build(const char* file, const Program& pg) {
    LevelHeader _header = {0};

    // read level file
    if (Level_ReadFile(file, _header, vtiles) < 0) {
        cerr << "Level_ReadFile: " << Level_GetError() << '\n';
        return 1;
    }

    // globalize header
    header = _header;

    // define camera dimensions
    {
        SDL_Rect temp = {0, 0, _header.width * TILE_WIDTH, _header.height * TILE_HEIGHT};
        camera::define(temp);
    }

    // create two textures to act as the tilemap layers
    if (! _newLevel(pg.renderer)) return 1;

    // create texture that "shades" between the two layers
    if (! _newShader(pg.renderer)) return 1;

    // load the background image
    bg->renderer = pg.renderer;
    if (! _newBackground(_header.background)) return 1;

    // load the tileset
    if (! _newTileset(_header.tileset, pg.renderer)) return 1;

    // build the tilemap based on the vector array
    _buildTilemap(pg.renderer);

    //
    using namespace std;
    cout << "Level: " << (int) header->width << 'x' << (int) header->height \
    << " tiles with background " << (int) header->background;
    cout << ", with tileset " << (int) header->tileset << ", and music " \
    << (int) header->music << endl;

return 0;
}

// Private Functions //
//PVector<int> PVmod(int& x, int& y) {
//    x = x & TILE_WIDTH;
//    y = y & TILE_HEIGHT;
//
//return PVector<int>(x, y);
//}

//uint16_t incrTile(uint16_t tile, const PVector<int>& incs) {
//    tile += incs.x() + incs.y() * header->width;
//
//return tile;
//}

void _buildTilemap(SDL_Renderer* ren) {
    SDL_Rect src = {0, 0, TILE_WIDTH, TILE_HEIGHT};
    SDL_Rect dst = {0, 0, TILE_WIDTH, TILE_HEIGHT};

    // width of the entire level
    const int imgw = header->width * TILE_WIDTH;

    // clear both layers
    for (int x = 0; x < 2; ++x) {
        break;
        SDL_SetRenderTarget(ren, layers[x]);
        SDL_RenderClear(ren);
    }

    for (uint16_t a = 0; a < vtiles.size(); ++a) {
        // tile a
        auto itr = vtiles.begin() + a;

        // 0 for lower tilemap, 1 for upper
        int layer = WhichLayer(itr->flags);

        // clear tilemap
        SDL_SetRenderTarget(ren, layers[layer]);

        // skip empty tiles
        if (! itr->graphicID) continue;

        // get the location of the source tile
        Point<int> p;
        p = getXY(256, itr->graphicID);
        src.x = p.x();
        src.y = p.y();

        // get the location of
        p = getXY(imgw, a);
        dst.x = p.x();
        dst.y = p.y();

        // copy tile to tilemap
        SDL_RenderCopy(ren, tileset, &src, &dst);
    }

    // do not render to a texture anymore
    SDL_SetRenderTarget(ren, nullptr);
}

bool _newTileset(const uint8_t id, SDL_Renderer* ren) {
    SDL_Surface* tsuf = nullptr;
    std::string file = "images/tilesets/";

    // file based on integer argument
    switch (id) {
    default:
    case 0:
        file += "000.png";
        break;

    case 1:
        file += "001.png";
        break;
    }

    // load surface
    bool retval;
    tsuf = IMG_Load(file.c_str());

    // save error in case it is needed
    file = IMG_GetError();

    // check if surface isn't NULL
    retval = tsuf != nullptr;

    if (retval) {
        std::cout << "retval is true" << endl;

        // convert surface to texture
        tileset = SDL_CreateTextureFromSurface(ren, tsuf);
        file += "...";
        file += SDL_GetError();

        // return value is true if texture is valid
        retval = tileset != nullptr;

        // free the surface and make it not point to anything
        SDL_FreeSurface(tsuf);
        tsuf = nullptr;
    }

    // if at any point retval has become false, print an error message
    if (! retval)
        cerr << "_newTileset: " << file << '\n';

return retval;
}

bool _newBackground(const uint8_t id) {
    // image file chosen based on id number
    switch (id) {
    case 0:
        bg->open("images/backgrounds/clear_sky.png");
        break;

    case 1:
        bg->open("images/backgrounds/grassy_field.png");
        break;

    default:
        cerr << "_newBackground: invalid id number " << (int) id << '\n';
        return false;
        break;
    }

    // process error if it happens
    if (bg->error()) {
        cerr << "_newBackground: " << bg->get_error() << '\n';
        return false;
    }

    // configure bg
    bg->set_blit_size(WIDTH, HEIGHT);

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

    constexpr uint8_t pattern[16] = {
        1, 0, 0, 1,
        0, 1, 1, 0,
        0, 1, 1, 0,
        1, 0, 0, 1
    };

    SDL_Surface* tempSurface = nullptr;

    // create new 16-bit surface
    tempSurface = SDL_CreateRGBSurfaceWithFormat(0, WIDTH, HEIGHT, 16, SDL_PIXELFORMAT_RGBA4444);
    if (tempSurface == nullptr) {
        cerr << "_newShader: " << SDL_GetError() << '\n';
        return false;
    }

    // map 16-bit color value
    const uint32_t color = SDL_MapRGBA(tempSurface->format, 0x55, 0x55, 0x55, 0xff);

    // array of pixels
    uint16_t* uippixels = (uint16_t*) tempSurface->pixels;

    /* Go down each row and change its pixels. [pttidx] is an index to
       the table at [pattern]. [pixel] is an index to an array
       representing the surface's pixel map. */
    for (int y = 0; y < tempSurface->h; ++y) {
        uint8_t pttidx = (y % 4) * 4;
        int pixel = tempSurface->w * y;

        // each pixel on a row
        for (int x = 0; x < tempSurface->w; ++x) {
            if (pattern[pttidx + x % 4])
                uippixels[pixel] = color;

            ++pixel;
        }
    }

    // convert surface to texture
    std::string error;
    layers[Shader] = SDL_CreateTextureFromSurface(ren, tempSurface);
    error = SDL_GetError();

    // free memory used by the surface
    SDL_FreeSurface(tempSurface);

    // return an error if the texture is null
    if (layers[Shader] == nullptr) {
        cerr << "SDL_CreateTextureFromSurface: " << error << '\n';
        return false;
    }

return true;
}

bool _newLevel(SDL_Renderer* ren) {
    // x = iTbl1[0]
    int iTbl1[] = {
        0,
        header->width * TILE_WIDTH,
        header->height * TILE_HEIGHT
    };

    // back and frontal layer are [0] and [1] respectively
    for (iTbl1[0] = 0; iTbl1[0] < 2; iTbl1[0]++) {
        short x = iTbl1[0]; // index to the layer table

        // create texture that can be rendered to
        layers[x] = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA4444,
                                      SDL_TEXTUREACCESS_TARGET, iTbl1[1], iTbl1[2]);
        if (layers[x] == nullptr) {
            cerr << "SDL_CreateTexture: " << SDL_GetError() << '\n';
            return false;
        }

        // enable transparency
        SDL_SetTextureBlendMode(layers[x], SDL_BLENDMODE_BLEND);
    }

return true;
}
