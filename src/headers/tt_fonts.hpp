#ifndef TT_FONTS_HPP
#define TT_FONTS_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <custom/generic_class.hpp>

#include "bitmap_font.hpp"
#include <custom/arrays.hpp>

#define FONT_COLOR_WHITE	0xff, 0xff, 0xff
#define FONT_COLOR_BLACK	0x00, 0x00, 0x00

struct FontColor {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	
	FontColor() : red(0), green(0), blue(0) {}
	FontColor(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}
};

struct Font {
	TTF_Font* font;
	uint16_t size;
	SDL_Surface* surface;
	
	void free();
	void close();
	
	void destroy() {
		free();
		close();
	}
	
	explicit Font() : font(nullptr), size(0), surface(nullptr) {}
	~Font() {destroy();}
	
	// returns true if the font is NOT loaded
	bool operator!() const {return font == nullptr;}
	
	// returns true if the font is loaded
	operator bool() const {return font != nullptr;}
	bool loaded() const {return font != nullptr;}
};

// initializes the SDL truetype font library. If it returns false, call TT_Font_GetError()
bool TT_Font_Init();

// deacitvates the font library
void TT_Font_Quit();

/* Loads a font file into fontObj.font. If it returns 0 it was successfull. If it returns 1 or -1,
   there is some kind of error; call TT_Font_GetError() for details. A return value of 1 means
   that TT_Font_Init wasn't called.
*/
int TT_Font_LoadFont(Font& fontObj, const char* file, const int ptsize);

/* Renders text to a surface using the specified font. Text is rendered using TTF_RenderText_Solid.
   If it returns 0 the operation was a success, else there is an error. And in the case of an error,
   call TT_Font_GetError() for details. If the return value is 1 in particular, it means TT_Font_Init
   wasn't called beforehand. It needs to be called for any of these functions to work, except
   TT_Font_GetError.
*/
int TT_Font_RenderText(Font& fontObj, const char* text, const FontColor& color);

/* Constructs a bitmap font using the true type font indicated by the second argument. If it returns 0, the
   operation was a success; however, a non-zero return value indicates some type of error. In such a case,
   TT_Font_GetError() will return a string detailing the error. As for the return value, there are two possible
   error codes: if the function returns 1, TT_Font_Init() was not called; -1 means some internal error had transpired.
*/
int TT_Font_RenderCharacters(SDL_Renderer* renderer, Font& fontObj, BitmapFont& bmpFont,
  const SDL_Color& fgcolor, const SDL_Color* bgcolor = nullptr);

// returns a SDL_Color struct based on the color value provided
SDL_Color TT_Font_MapRGB(uint32_t color);

#endif /* TT_FONTS_HPP */
