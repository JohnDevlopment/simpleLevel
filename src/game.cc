#include "headers/game.hpp"
#include "headers/memory.hpp"

// special defines
#define NumberOfSaveBytes	3

// namespace globals
std::vector<Image>		game::Graphics(NUM_IMAGES);
generic_class<BitmapFont>	game::Bitmap;
Bitfield<uint16_t>		game::Flags;
uint8_t				game::FrameCounter = 0;
uint32_t			game::MiscTimer = 0;
uint32_t*			game::SaveData = nullptr;

static SDL_Renderer* _private_renderer = nullptr;

using namespace game;

// public namespace member functions
bool game::loadMedia(SDL_Renderer* ren) {
	// define the renderer for each Image
	for (int x = 0; x < NUM_IMAGES; ++x)
	  Graphics[x].renderer = ren;
	
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
	if (! alloc_mem(SaveData, NumberOfSaveBytes))
	  return false;
	
	// open an image file and build a font off of it
	if (Bitmap->open(ren, "images/fonts/dialogue/dialogue.png", 0xffffffff))
	  Bitmap->build("images/fonts/dialogue/dialogue.bin");
	
	_private_renderer = ren;
	
return true;
}

void game::free() {
	Graphics.clear();
	delete[] SaveData;
	Bitmap->free();
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
