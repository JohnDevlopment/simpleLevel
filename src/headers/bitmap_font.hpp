#ifndef BITMAP_FONT_HPP
#define BITMAP_FONT_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cstring>

#include "global_attributes.hpp"

struct BitmapFont_Pixels {
	int row;
	int col;
	int x;
	int y;
};

/*
typedef Struct {
	int x;
	int y;
	int w;
	int h;
} SDL_Rect;
*/

class BitmapFont {
	// texture information
	SDL_Texture* texture;
	int width;
	int height;
	
	// font measurements
	int cellHeight;
	int cellWidth;
	int top;
	int baseA;
	uint16_t space;
	uint16_t newline;
	SDL_Rect fontChars[256];
	
	// move function
	void move(BitmapFont&& rhs);
	
	public:
	
	BitmapFont();
	
	BitmapFont(BitmapFont&& rhs) {this->move( std::move(rhs) );}
	
	~BitmapFont() {free();}
	
	// get location of a cell marked by the index
	SDL_Point cellNumber(const int idx);
	
	// get texture
	SDL_Texture* get() const {return texture;}
	
	// load image
	bool open(SDL_Renderer* renderer, std::string file, uint32_t colorkey);
	
	// build a registry of each character in the font and how to render it
	bool build(std::string file);
	
	// render text to the screen
	void render(SDL_Renderer* renderer, const std::string& str, int x, int y, int line = -1);
	
	// free texture
	void free();
	
	// move assignment
	void operator=(BitmapFont&& rhs) {this->move( std::move(rhs) );}
};

//SDL_Texture* Bitmap_LoadImage(SDL_Renderer* renderer, std::string file, uint32_t colorkey);

uint32_t Bitmap_GetPixel32(void* pixels, int pitch, unsigned int x, unsigned int y) PURE;

//bool Bitmap_BuildBitmapFont(BitmapFont& font);

//void Bitmap_RenderText(const BitmapFont& font, SDL_Renderer* renderer, int x, int y, std::string str);

//unsigned int Bitmap_StringLength(const BitmapFont& font, std::string text);

#endif /* BITMAP_FONT_HPP */
