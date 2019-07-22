// global game data
#include "game.hpp"

#include "levelcode.hpp"
#include "levelinfo.hpp"
#include "sound.hpp"
#include "camera.hpp"
#include "triggers.hpp"
#include "tcl.hpp"
#include "log.hpp"
#include "tile_collision.hpp"
#include "gm_level.hpp"
#include "particle_sprite_def.h"
#include <bad_option>
#include <lvalue_rvalue_pointers.hpp>
#include <sstream>
#include <arrays.hpp>

#define NO_KEY_SYM_BITMASKS 1
#include "private/player_data_def.h"

// private variables

// private function prototypes
//static bool loadBackground();
static bool loadTileset(SDL_Surface**);
static bool createTilemaps(SDL_Surface**);
static void buildLevel(SDL_Surface*, SDL_Surface**);
static bool turnTilemapsToTexture(SDL_Surface** const, SDL_Renderer*);
static void updateBackground(SDL_Rect* const);
static void updateSpritesAndTilemap(const bool, void*);
static SDL_Rect&& getXY(const SDL_Surface*, uint16_t);
static void placeTile(const Tile&, SDL_Surface*, uint16_t, SDL_Surface**);
static int flagsBasedOnCodeID(const Tile&);

using namespace std;
using namespace level;

// initialize namespace members //
// tiles of the level
std::vector<Tile>		level::VTiles;

// level header
generic_class<LevelHeader>	level::Header;

// tilemaps
SDL_Texture* 			level::Tilemap[4]	= {nullptr, nullptr, nullptr, nullptr};

// defines the player's attributes
Player* 			level::ThePlayer	= nullptr;

// background offset
int 				level::bgx		= 0;

// level event index
uint8_t				level::levelEvents	= GMLevel_Normal;

// screen exits
std::vector<std::vector<char>>	level::screenExits;

// public member functions
void level::correctBackground(const SDL_Rect* srcCam, const SDL_Rect* dstCam) {
	static int iDiff = 0;
	
	// record old camera difference
	if (srcCam != nullptr)
	  iDiff = bgx - srcCam->x; // d[bcx] = x[b] - x[c]
	
	// use difference to change the background such that it renders in the relative position
	if (dstCam != nullptr)
	  bgx = iDiff + dstCam->x; // x[b] = d[bcx] + x[c]
}

void level::exit(int x, int y) {
	// X = second byte of X offset. Y = Y offset divided by hald the level height (in pixels)
	{
	  const int iHalfHeight = Header->height / 2 * TILE_HEIGHT;
	  
	  x = (x & (int) 0xff00) >> 8;
	  y /= iHalfHeight;
	}
	
	// two exits per screen, so y times two plus x equals the array index
	const unsigned int index = x * 2 + y;
	
	if (index < screenExits.size() ) {
	  const auto& rElm = screenExits[index];
	  string level = (const char*) rElm.data();
	  
	  // eval command
	  Tcl_Obj* cmd = Tcl_NewStringObj("set CurrentLevel ", -1);
	  Tcl_AppendToObj(cmd, (const char*) rElm.data(), rElm.size() );
	  
	  if ( Tcl_EvalObjEx(gInterp, cmd, 0) != TCL_OK )
	  	Log_Cerr("Error from Tcl_EvalObjEx: %s\n", Tcl_GetStringResult(gInterp) );
	  
	  // printf value
	  printf("Value of CurrentLevel: %s\n", CurrentLevel);
	}
}

int level::load(string file, const PROGRAM& program) {
	// create the player
	if (ThePlayer == nullptr) {
	  // initialize a Player object
	  ThePlayer = new Player(program.renderer);
	}
	
	// check the length of the file string
	if (file.length() == 0) {
	  cerr << "No file name given.\n";
	  return -1;
	}
	
	// read level into memory
	{
	  int retval = Level_ReadFile(file.c_str(), Header.get(), VTiles);
	  
	  if (retval <= 0) {
	  	cerr << "level::load: could not read " << file << ": " << Level_GetError() << '\n';
	  	return 1;
	  }
	}
	
	// load background image
//	if (! loadBackground()) return 1;
	
	// tileset surface
	SDL_Surface* temp_surTileset = nullptr;
	
	// load tileset image into surface
	if (! loadTileset(&temp_surTileset)) return 1;
	
	// two new tilemap surfaces
	SDL_Surface* temp_surTilemaps[2] = {nullptr, nullptr};
	
	if (! createTilemaps(temp_surTilemaps)) {
	  // free the tileset
	  SDL_FreeSurface(temp_surTileset);
	  
	  // free the first element of temp_surTilemaps
	  SDL_FreeSurface(temp_surTilemaps[0]);
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
	
return 0;
}

void level::update(GameMode* gm, void* udata, char entry) {
	if (! entry) {
	  // camera control
	  camera::track();
	}
	
	// render graphics a certain way according to the argument
	switch (entry) {
	  case 1:
	  	updateBackground(&CAM_CAMERA);
	  	updateSpritesAndTilemap(false, udata);
	  	break;
	  
	  case 0:
	  	updateBackground(&CAM_CAMERA);
	  	updateSpritesAndTilemap(true, udata);
	  	break;
	  
	  default:
	  	{
	  	  char error[50];
	  	  std::sprintf(error, "Invalid argument to level::update, %d", (int) entry);
	  	  throw bad_option(error);
	  	  break;
	  	}
	}
}

void level::unload() {
	// fill header with zeroes
	std::memset(Header.getp(), 0, sizeof(LevelHeader));
	
	// free array of tiles
	VTiles.clear();
	
	// unload tilemaps
	Free(&TILEMAP_BACK, &TILEMAP_FRONT);
	
	// reset background position
	bgx = 0;
	
	// backgrounds
	BG_BG2.unload();
	BG_BG1.unload();
}

// private helper functions
void updateSpritesAndTilemap(const bool willRenderSprites, void* udata) {
	SDL_Rect* cam = &CAM_CAMERA;
	
	// render the back tilemap
	SDL_RenderCopy(ThePlayer->m_obj.renderer, TILEMAP_BACK, cam, nullptr);
	
	CurrentSprite = ThePlayer;
	
	if (ThePlayer->Main(udata) == 1) {
	  game::Flags.set(FADEOUT | QUITGAME);
	  camera::Track = false;
	}
	
	// render the front tilemap
	SDL_RenderCopy(ThePlayer->m_obj.renderer, TILEMAP_FRONT, cam, nullptr);
}

void updateBackground(SDL_Rect* const camera) {
	using camera::BGMinX;
	using camera::BGMinY;
	
	// render background(s)
	{ 
	  auto itr = camera::BGLayers.begin();
	  short int x = 0;
	  
	  while (itr != camera::BGLayers.end()) {
	  	Background_Base* ptr = *itr;
	  	
	  	// call the background class' render function
	  	if (ptr) {
	  	  ptr->check(camera); // calculates whether camera is within background's rect
	  	  Point<int> bgpos = ptr->get_position(); // location of the background
	  	  
	  	  // render background
	  	  Image* pImg = LevelInfo_BackgroundLayerID(x);
	  	  
	  	  pImg->blit(bgpos.x(),         bgpos.y(), camera);
	  	  pImg->blit(bgpos.x() + WIDTH, bgpos.y(), camera);
	  	  ++x;
	  	}
	  	else {
	  	  Image* pImg = LevelInfo_BackgroundLayerID(x);
	  	  pImg->blit(0, 0, camera);
	  	  pImg->blit(WIDTH, 0, camera);
	  	  ++x;
	  	}
	  	
	  	++itr;
	  }
	}
}

void buildLevel(SDL_Surface* surTileset, SDL_Surface** surTilemaps) {
	// copy all tiles onto the tilemap
	auto itr = VTiles.begin();
	
	for (uint16_t a = 0; a < VTiles.size(); ++a) {
	  // change the bit flags on the tile based on its code ID
	  itr->flags = flagsBasedOnCodeID(*itr);
	  
	  // place the tile onto the bitmap
	  placeTile(*itr, surTileset, a, surTilemaps);
	  ++itr;
	}
}

int flagsBasedOnCodeID(const Tile& tile) {
	int flags = tile.flags;
	
	// slopes between 8 - 11
	int iSlpID = tile.codeID - 8;
	
	if (iSlpID >= 0 && iSlpID < NUM_SLOPES)
	  flags |= TILEFLAG_ToSlopeID(iSlpID) | TILEFLAG_SLOPE;
	
	// slopes between 12 - 15
	iSlpID = tile.codeID - 12;
	
	if (iSlpID >= 0 && iSlpID < NUM_SLOPES)
	  flags |= TILEFLAG_ToSlopeID(iSlpID) | TILEFLAG_SLOPEINV | TILEFLAG_SLOPE;
	
	// which tiles are solid
	if (tile.codeID == 25) {
	  flags |= TILEFLAG_SOLID;
	}
	
return flags;
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
	
	{
	  const char* temp_sFile = LevelInfo_Tileset(Header->tileset);
	  sFile += temp_sFile;
	}
	
	// load surface from file
	*surface = IMG_Load(sFile.c_str());
	
	if (*surface == nullptr) {
	  cerr << "Failed to load " << sFile.c_str() << ": " << IMG_GetError() << '\n';
	  return false;
	}
	
return true;
}

//bool loadBackground() {
//	StaticDArray<char,50> buffer;
//	const char* sFile = LevelInfo_Background(Header.getc());
//	
//	// if the background is already loaded, then return
//	if (BG_BG1.loaded()) return true;
//	
//	// file name can be 30 characters long
//	if (strlen(sFile) > 30) {
//	  // concat an error string
//	  StaticDArray<char,256> error;
//	  sprintf(error.data(), "\"%s\" is too long (%u characters), maximum is 30", sFile, (size_t) strlen(sFile));
//	  
//	  // throw an exception
//	  throw bad_option (error.cbegin());
//	}
//	
//	// abstract file string
//	strcpy(buffer.data(), "images/backgrounds/");
//	strcat(buffer.data(), sFile);
//	
//	// load background
//	if (! BG_BG1.open(buffer.data())) {
//	  cerr << "Failed to load " << buffer.data() << ": " << BG_BG1.get_error() << '\n';
//	  return false;
//	}
//	
//return true;
//}

SDL_Rect&& getXY(const SDL_Surface* surface, uint16_t tileNum) {
	// return value
	SDL_Rect retval = {0, 0, TILE_WIDTH, TILE_HEIGHT};
	
	// reduce X and Y to tile offsets
	int x = TILE_WIDTH * tileNum;
	retval.x = x - surface->w * (x / surface->w);
	retval.y = x / surface->w * TILE_HEIGHT;
	
return std::move(retval);
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
	// nothing to do with empty source tile
	if (tile.graphicID == 0) return;
	
	// location to blit the tile
	uint8_t layer = TILEFLAG_GetLayer(tile);
	SDL_Rect destination = getXY(surTilemaps[layer], destNum);
	
	// tile to copy
	SDL_Rect source = getXY(surTileset, tile.graphicID);
	
	// copy tile to tilemap
	SDL_BlitSurface(surTileset, &source, surTilemaps[layer], &destination);
}
