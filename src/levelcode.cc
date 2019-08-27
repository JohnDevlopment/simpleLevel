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
#include <bad_option>
#include <sstream>

// custom hears
#include <arrays.hpp>
#include <lvalue_rvalue_pointers.hpp>

#include "log.hpp"
#include "game.hpp"
#include "levelcode.hpp"
#include "levelinfo.hpp"
#include "sound.hpp"
#include "camera.hpp"
#include "triggers.hpp"
#include "tcl.hpp"
#include "tile_collision.hpp"
#include "gm_level.hpp"
#include "particle_sprite_def.h"

// private function prototypes | code: INTFUNC_DECL
static void makeTileLocList(const int);
static bool loadBackground();
static bool loadTileset(SDL_Surface**);
static bool createTilemaps(SDL_Surface**);
static void buildLevel(SDL_Surface*, SDL_Surface**);
static bool turnTilemapsToTexture(SDL_Surface** const, SDL_Renderer*);
//static void updateBackground(SDL_Rect* const);
//static void updateSpritesAndTilemap(SDL_Renderer* const);
static SDL_Rect getXY(const int, uint16_t);
static void placeTile(const Tile&, SDL_Surface*, uint16_t, SDL_Surface**);
static int flagsBasedOnCodeID(const Tile&);

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
	if (! loadBackground())
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
//	updateBackground(&CAM_CAMERA);
//	updateSpritesAndTilemap();
	
	SDL_Rect* cam = &CAM_CAMERA;
	
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
	
	// unload tilemaps
	Free(&TILEMAP_BACK, &TILEMAP_FRONT);
	
	// reset background position
	BGX = 0;
	
	// unload background images
	if (! Flags.mask(DONT_UNLOAD_BGS)) {
	  BG_BG2.unload();
	  BG_BG1.unload();
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


//void updateSpritesAndTilemap(SDL_Renderer* const ren) {
//	SDL_Rect* cam = &CAM_CAMERA;
//	
//	// render back tilemap
//	SDL_RenderCopy(ren, TILEMAP_BACK, cam, nullptr);
//	
//	// update the player`s data and graphics
//	CurrentSprite = ThePlayer;
//	if (ThePlayer->Main(udata) == 1) {
//	  game::Flags.set(FADEOUT|QUITGAME);
//	  camera::Track = false;
//	}
//	
//	// render front tilemap
//	SDL_RenderCopy(ren, TILEMAP_FRONT, cam, nullptr);
//}

//void updateBackground(SDL_Rect* const camera) {
//	using camera::BGMinX;
//	using camera::BGMinY;
	
	// render background(s)
//	{ 
//	  auto itr = camera::BGLayers.begin();
//	  short int x = 0;
//	  
//	  while (itr != camera::BGLayers.end()) {
//	  	Background_Base* ptr = *itr;
//	  	
//	  	// call the background class' render function
//	  	if (ptr) {
//	  	  ptr->check(camera); // calculates whether camera is within background's rect
//	  	  Point<int> bgpos = ptr->get_position(); // location of the background
//	  	  
//	  	  // render background
//	  	  Image* pImg = LevelInfo_BackgroundLayerID(x);
//	  	  
//	  	  pImg->blit(bgpos.x(),         bgpos.y(), camera);
//	  	  pImg->blit(bgpos.x() + WIDTH, bgpos.y(), camera);
//	  	  ++x;
//	  	}
//	  	else {
//	  	  Image* pImg = LevelInfo_BackgroundLayerID(x);
//	  	  pImg->blit(0, 0, camera);
//	  	  pImg->blit(WIDTH, 0, camera);
//	  	  ++x;
//	  	}
//	  	
//	  	++itr;
//	  }
//	}
//}

void buildLevel(SDL_Surface* surTileset, SDL_Surface** surTilemaps) {
	Tile* aTile = Tiles;
	const uint16_t uiSize = Header->width * Header->height;
	
	for (uint16_t a = 0; a < uiSize; ++a) {
	  int iFlags = flagsBasedOnCodeID(*aTile);
	  
	  aTile->flags = (uint16_t) iFlags;
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

bool loadBackground() {
	std::string buffer = "images/backgrounds/";
	const char* sFile = LevelInfo_Background(Header->background);
	
	// if the background is already loaded, then return
	if (BG_BG1.loaded())
	  return true;
	
	buffer += sFile;
	
	// load background
	if (! BG_BG1.open(buffer.c_str())) {
	  cerr << "Failed to load " << buffer << ": " << BG_BG1.get_error() << '\n';
	  return false;
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
