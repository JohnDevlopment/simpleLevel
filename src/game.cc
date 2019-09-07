#include "game.hpp"
#include "log.hpp"
#include "memory.hpp"
#include "string.h"

// special defines
#define NumberOfSaveBytes	3

// namespace globals
Image*				game::Graphics = nullptr;
generic_class<BitmapFont>	game::Bitmap;
Bitfield<uint16_t>		game::Flags;
uint8_t				game::FrameCounter = 0;
uint32_t			game::MiscTimer = 0;
uint32_t*			game::SaveData = nullptr;
uint32_t			game::KeySymBits = 0;
uint32_t			game::KeySymBitsFirstFrame = 0;

static SDL_Renderer* _private_renderer = nullptr;

using namespace game;

// public namespace member functions
/* Includes a header that defines game::makeHash. */
#define HASHFUNC_H_IMPLEMENTATION 1
#define HASHFUNC_H_NS game
#include "hashfunc.h"

SDL_Texture* game::loadTexture(SDL_Renderer* ren, const char* file, const uint32_t* key) {
	SDL_Texture* tex = nullptr;
	SDL_Surface* suf;
	
	// load image
	suf = IMG_Load(file);
	if (suf == nullptr) {
	  PrintError(game::loadTexture, IMG_GetError());
	  return nullptr;
	}
	
	// optional colorkey
	if (key) {
	  uint32_t uiColor = *key;
	  uiColor = SDL_MapRGB(suf->format, uiColor >> 16, uiColor >> 8, uiColor);
	  if ( SDL_SetColorKey(suf, SDL_TRUE, uiColor) < 0 ) {
	  	PrintError(game::loadTexture, SDL_GetError());
	  }
	}
	
	// convert into texture
	tex = SDL_CreateTextureFromSurface(ren, suf);
	if (tex == nullptr) {
	  PrintError(game::loadTexture, SDL_GetError());
	}
	
	SDL_FreeSurface(suf);
	
return tex;
}

bool game::loadMedia(SDL_Renderer* ren) {
	Log_Cout("### Loading Global Media ###\n");
	
	// allocate array
	if (Graphics == nullptr) {
	  Graphics = new Image[NUM_IMAGES];
	  
	  // define the renderer for each Image
	  for (int x = 0; x < NUM_IMAGES; ++x)
	  	Graphics[x].renderer = ren;
	}
	
	// black rectangle image
	if (! BG_BLACK.open("images/ui/black.bmp")) {
	  std::cerr << "Failed to load black.bmp: " << BG_BLACK.get_error() << '\n';
	  return false;
	}
	
	// configure the size and transparency of the image
	BG_BLACK.set_blit_size(WIDTH, HEIGHT);
	BG_BLACK.set_blend_mode(SDL_BLENDMODE_BLEND);
	
	// dialogue box image
	if (! BG_DIALOGUEBOX.open("images/ui/dbox.png")) {
	  std::cerr << "Failed to load dbox.png: " << BG_DIALOGUEBOX.get_error() << '\n';
	  return false;
	}
	
	// allocate an array of integers that represent the game's save data
//	if (! alloc_mem(SaveData, NumberOfSaveBytes))
//	  return false;
	
	// open an image file and build a font off of it
//	if (Bitmap->open(ren, "images/fonts/dialogue/dialogue.png", 0xffffffff))
//	  Bitmap->build("images/fonts/dialogue/dialogue.bin");
	
	_private_renderer = ren;
	
	{
	  SDL_Rect temp = BG_BLACK.get_file_rect();
	  Log_Cout("Loaded black.bmp: width = %d, height = %d. Texture will blit at %dx%d and have alpha blending\n",
	  	temp.w, temp.h, WIDTH, HEIGHT);
	  
	  temp = BG_DIALOGUEBOX.get_file_rect();
	  Log_Cout("Loaded dbox.png: width = %d, height = %d\n", temp.w, temp.h);
	  
	  Log_Cout("Allocated %d bytes of potential save data\n", (int) NumberOfSaveBytes);
	}
	
	Log_Cout("Initialized global game resources\n\n");
	
return true;
}

void game::free() {
	delete[] Graphics;
	delete[] SaveData;
//	Bitmap->free();
}

void game::bgcolor(const JColor& color) {
	SDL_SetRenderDrawColor(_private_renderer, color.red, color.green, color.blue, color.alpha);
}

Point<int>& operator<<(Point<int>& point, SDL_Rect& rect) {
	point[0] = rect.x;
	point[1] = rect.y;
	
return point;
}

Point<int>& operator>>(Point<int>& point, SDL_Rect& rect) {
	rect.x = point.x();
	rect.y = point.y();
	
return point;
}

SDL_Rect operator-(const SDL_Rect& A, const SDL_Rect& B) {
	SDL_Rect C = {A.x - B.x, A.y - B.y, A.w - B.w, A.h - B.h};
	
return C;
}
