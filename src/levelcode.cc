/* * File = levelcode.cc
   * Project = simpleLevel
   * File type = source
   * Description: This source file is part of the project simpleLevel. It contains
   * definitions for the functions listed in the associated header.
   * For navigation purposes, here are some search keywords:
   *   - code INTFUNC_DECL for private function declarations
   *   - code INTFUNC_IMPL for private function implementations
   *   - code EXTFUNC_IMPL for external function implementations
   *   - code ENSVARS for external namespace variables being defined
   * 
*/

// system headers
#include <algorithm>
#include <arrays.hpp>
#include <bad_option>
#include <lvalue_rvalue_pointers.hpp>
#include <sstream>

#include "camera.hpp"
#include "game.hpp"
#include "gm_level.hpp"
#include "levelcode.hpp"
#include "levelinfo.hpp"
#include "log.hpp"
#include "math.hpp"
#include "particle_sprite_def.h"
#include "sound.hpp"
#include "string.h"
#include "tcl.hpp"
#include "tile_collision.hpp"
#include "triggers.hpp"

// private function prototypes | code: INTFUNC_DECL
static void makeTileLocList(const int);
static bool loadBackground(SDL_Renderer* const);
static bool loadTileset(SDL_Surface**);
static bool createTilemaps(SDL_Surface**);
static void buildLevel(SDL_Surface*, SDL_Surface**);
static bool turnTilemapsToTexture(SDL_Surface** const, SDL_Renderer*);
static SDL_Rect getXY(const int, uint16_t);
static void placeTile(const Tile&, SDL_Surface*, uint16_t, SDL_Surface**);
static int flagsBasedOnCodeID(const Tile&);
static void renderBackground(SDL_Renderer* const, const SDL_Rect*);

// import namespaces
using namespace std;
using namespace level;

// initialize namespace members | code: ENSVARS
generic_class<LevelHeader> level::Header;
SDL_Texture* level::Tilemap[4];
Player* level::ThePlayer;
int level::BGX;
Tile* level::Tiles = nullptr;
char* level::CurrentLevel = nullptr;
Point<int>* level::TileLocations = nullptr;
SDL_Texture* level::Backgrounds[2];

// public member functions | code: EXTFUNC_IMPL
//void level::correctBackground(const SDL_Rect* srcCam, const SDL_Rect* dstCam) {
//	static int iDiff = 0;
//	
//	// record old camera difference
//	if (srcCam != nullptr)
//	  iDiff = bgx - srcCam->x; // d[bcx] = x[b] - x[c]
//	
//	// use difference to change the background such that it renders in the relative position
//	if (dstCam != nullptr)
//	  bgx = iDiff + dstCam->x; // x[b] = d[bcx] + x[c]
//}

int level::load(string file, const PROGRAM& program) {
	SDL_Surface* temp_surTileset = nullptr;
	SDL_Surface* temp_surTilemaps[2] = {nullptr, nullptr};
	
	// create the player
	if (ThePlayer == nullptr) {
	  ThePlayer = new Player(program.renderer);
	}
	
	// check the length of the file string
	if (file.length() == 0) {
	  cerr << "No file name given.\n";
	  return -1;
	}
	
	// read level into memory
	{
	  int retval = Level_ReadFile(file.c_str(), Header.getp(), &Tiles, nullptr);
	  
	  if (retval <= 0) {
	  	cerr << "level::load: could not read " << file << ": " << Level_GetError() << '\n';
	  	return 1;
	  }
	}
	
	// load background image
	if (! loadBackground(program.renderer))
	  return 1;
	
	// load tileset image into surface
	if (! loadTileset(&temp_surTileset))
	  return 1;
	
	// create empty tilemap surfaces
	if (! createTilemaps(temp_surTilemaps)) {
	  Free(&temp_surTileset, &temp_surTilemaps[0]/*, &temp_surTilemaps[1]*/);
	  return 1;
	}
	
	// build the tilemaps using VTiles as a guide to which of the tileset to put on the tilemaps
	buildLevel(temp_surTileset, temp_surTilemaps);
	
	// convert the tilemap surfaces into textures
	if (! turnTilemapsToTexture(temp_surTilemaps, program.renderer)) {
	  Free(&temp_surTilemaps[0], &temp_surTilemaps[1], &temp_surTileset);
	  return 1;
	}
	
	// free the back and front tilemap surfaces and the tileset surface
	Free(&temp_surTilemaps[0], &temp_surTilemaps[1], &temp_surTileset);
	
	// define the level boundaries
	camera::defineLevel(Header->width * TILE_WIDTH, Header->height * TILE_HEIGHT);
	
	// create list of coordinates for each tile
	makeTileLocList(Header->width * Header->height);
	
return 0;
}

void level::update(GameMode* gm, SDL_Renderer* const ren) {
	camera::track();
	
	SDL_Rect* cam = &CAM_CAMERA;
	
	// render backgrounds relative to the camera
	renderBackground(ren, cam);
	
	// render back tilemap
	SDL_RenderCopy(ren, TILEMAP_BACK, cam, nullptr);
	
	// update the player`s data and graphics
	CurrentSprite = ThePlayer;
	if (ThePlayer->Main(nullptr) == 1) {
	  game::Flags.set(FADEOUT|QUITGAME);
	  camera::Track = false;
	}
	
	// render front tilemap
	SDL_RenderCopy(ren, TILEMAP_FRONT, cam, nullptr);
}

void level::unload() {
	using game::Flags;
	
	// clear data
	std::memset(Header.getp(), 0, sizeof(LevelHeader));
	
	// free array of tiles and its coordinate array
	delete[] Tiles;
	Tiles = nullptr;
	
	delete[] TileLocations;
	TileLocations = nullptr;
	
	// unload tilemaps; free background images
	Free(&TILEMAP_BACK, &TILEMAP_FRONT);
	
	// reset background position
	BGX = 0;
	
	// unload background images
	if (! Flags.mask(DONT_UNLOAD_BGS)) {
	  Free(&Backgrounds[0], &Backgrounds[1]);
	}
	else
	  Flags.unset(DONT_UNLOAD_BGS);
}

// private helper functions | code: INTFUNC_IMPL
void makeTileLocList(const int ntiles) {
	Point<int> tilexy;
	
	TileLocations = new Point<int>[ntiles];
	for (int x = 0; x < ntiles; ++x) {
	  tilexy = GetXY(x, Header->width); // tile_collision.o
	  TileLocations[x] = tilexy;
	}
}

void buildLevel(SDL_Surface* surTileset, SDL_Surface** surTilemaps) {
	Tile* aTile = Tiles;
	const uint16_t uiSize = Header->width * Header->height;
	
	for (uint16_t a = 0; a < uiSize; ++a) {
	  int iFlags = flagsBasedOnCodeID(*aTile);
	  
	  aTile->flags = (uint8_t) iFlags;
	  placeTile(*aTile, surTileset, a, surTilemaps);
	  ++aTile;
	}
}

int flagsBasedOnCodeID(const Tile& tile) {
	int iFlags = 0, iSlpID;
	
//	iFlags = tile.flags;
	
	// slopes between 8 - 11
	iSlpID = tile.codeID - 8;
	
	if (iSlpID >= 0 && iSlpID < NUM_SLOPES) {
	  iFlags = iFlags | (TILEFLAG_ToSlopeID(iSlpID) | TILEFLAG_SLOPE);
	}
	
	// slopes between 12 - 15
	iSlpID = tile.codeID - 12;
	
	if (iSlpID >= 0 && iSlpID < NUM_SLOPES) {
	  iFlags = iFlags | (TILEFLAG_ToSlopeID(iSlpID) | TILEFLAG_SLOPEINV | TILEFLAG_SLOPE);
	}
	
	// which tiles are solid
	if ( TILEFLAG_IsSlope(tile) ) {
	  iFlags |= TILEFLAG_SOLID;
	}
	else {
	  switch (tile.codeID) {
	  	default: break;
	  	
	  	case 1:
	  	case 25:
	  	case 27:
	  	  iFlags |= TILEFLAG_SOLID;
	  	  break;
	  }
	}
	
return iFlags;
}

bool createTilemaps(SDL_Surface** surfaces) {
	const char* sWhichLayers[] = {"back", "front"};
	
	// load the two tilemaps
	for (short x = 0; x < 2; ++x) {
	  // create a surface with X width and Y height, and a 16-bit color depth
	  surfaces[x] = SDL_CreateRGBSurfaceWithFormat(0, Header->width * TILE_WIDTH,
	                                               Header->height * TILE_HEIGHT, 32,
	                                               SDL_PIXELFORMAT_RGBA8888);
	  
	  // generate an error if the surface could not be created
	  if (surfaces[x] == nullptr) {
	  	cerr << "Failed to create surface for the " << sWhichLayers[x] \
	  	<< " tilemap: " << SDL_GetError() << '\n';
	  	return false;
	  }
	}
	
return true;
}

bool loadTileset(SDL_Surface** surface) {
	// concat file string
	string sFile = "images/tilesets/";
	sFile += LevelInfo_Tileset(Header->tileset);
	
	// load surface from file
	*surface = IMG_Load(sFile.c_str());
	
	if (*surface == nullptr) {
	  cerr << "Failed to load " << sFile.c_str() << ": " << IMG_GetError() << '\n';
	  return false;
	}
	
return true;
}

bool loadBackground(SDL_Renderer* const ren) {
	using game::loadTexture;
	using std::copy;
	
	constexpr int _sBuffer_len = 45;
	
	char sBuffer[_sBuffer_len];
	const char *sSpace, *sFileStr;
	SDL_Texture* texture;
	
	// backgrounds were not unloaded or the header bg index is zero
	if (Backgrounds[0] || Backgrounds[1] || ! Header->background)
	  return true;
	
	// base file string based on bg index
	sFileStr = LevelInfo_Background(Header->background);
	
	// append first substring to the directory prefix
	String_strcpy(sBuffer, "images/backgrounds/");
	sSpace = String_strchr(sFileStr, ' ');
	
	if (sSpace == nullptr) {
	  String_strlcat(sBuffer, sFileStr, _sBuffer_len);
	}
	else {
	  std::memset(sBuffer+19, 0, _sBuffer_len-19);
	  copy((char*) sFileStr, (char*) std::min((int) sFileStr+26, (int) sSpace), sBuffer+19);
	}
	
	// load first texture from file
	std::cout << "Loading " << sBuffer << std::endl;
	texture = loadTexture(ren, sBuffer, nullptr);
	if (! texture) {
	  Log_Cerr("Failed to load %s\n", sBuffer+19);
	}
	Backgrounds[0] = texture;
	
	// append second substring
	if (sSpace) {
	  sBuffer[19] = '\0';
	  String_strlcat(sBuffer, sSpace+1, _sBuffer_len);
	  std::cout << "Loading " << sBuffer << std::endl;
	  texture = loadTexture(ren, sBuffer, nullptr);
	  if (! texture) {
	  	Log_Cerr("Failed to load %s\n", sBuffer+19);
	  }
	  Backgrounds[1] = texture;
	}
	
return true;
}

SDL_Rect getXY(const int imgw, uint16_t tileNum) {
	SDL_Rect retval = {0, 0, TILE_WIDTH, TILE_HEIGHT};
	int x = TILE_WIDTH * tileNum;
	
	retval.x = x - imgw * (x / imgw);
	retval.y = x / imgw * TILE_HEIGHT;
	
return retval;
}

bool turnTilemapsToTexture(SDL_Surface** const surTilemaps, SDL_Renderer* ren) {
	// make back tilemap a texture
	TILEMAP_BACK = SDL_CreateTextureFromSurface(ren, surTilemaps[0]);
	
	if (TILEMAP_BACK == nullptr) {
	  cerr << "Failed to convert the back tilemap to texture: " \
	  << SDL_GetError() << endl;
	  return false;
	}
	
	// make front tilemap a texture
	TILEMAP_FRONT = SDL_CreateTextureFromSurface(ren, surTilemaps[1]);
	
	if (TILEMAP_FRONT == nullptr) {
	  cerr << "Failed to convert the front tilemap to texture: " \
	  << SDL_GetError() << endl;
	  return false;
	}
	
return true;
}

void placeTile(const Tile& tile, SDL_Surface* surTileset, uint16_t destNum, SDL_Surface** surTilemaps) {
	SDL_Rect destination, source;
	uint8_t uiLayer;
	
	// nothing to do with empty source tile
	if (tile.graphicID == 0)
	  return;
	
	// location to blit the tile
	uiLayer = TILEFLAG_GetLayer(tile);
	destination = getXY(surTilemaps[uiLayer]->w, destNum);
	
	// location of the source tile
	source = getXY(surTileset->w, tile.graphicID);
	
	// copy tile to tilemap
	SDL_BlitSurface(surTileset, &source, surTilemaps[uiLayer], &destination);
}

void renderBackground(SDL_Renderer* const ren, const SDL_Rect* camera) {
  const _BackgroundLayer* bg = camera::getbglayers();
	SDL_Rect dest = {0, 0, WIDTH, HEIGHT};
	
	// render backgrounds 0 & 1
	for (int x = 0; x < 2; ++x) {
	  if (Backgrounds[x]) {
	  	const SDL_Rect& bgrect = bg->rect;
	  	
	  	dest.x = bgrect.x - camera->x;
	  	dest.y = bgrect.y - camera->y;
	  	
	  	SDL_RenderCopy(ren, Backgrounds[x], nullptr, &dest);
	  	dest.x += WIDTH;
	  	SDL_RenderCopy(ren, Backgrounds[x], nullptr, &dest);
	  }
	  ++bg;
	}
}
