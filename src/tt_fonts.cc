// global game data
#include "headers/game.hpp"

// font system
#include "headers/tt_fonts.hpp"

#include <cmath>
#include <vector>

#define INIT_ERROR	"the font API has not been initialized"

using namespace std;

static bool Init = false;

// private helper functions //


// public member functions: Font //
void Font::free() {
	if (surface != nullptr) {
	  SDL_FreeSurface(surface);
	  surface = nullptr;
	}
}

void Font::close() {
	if (font != nullptr) {
	  TTF_CloseFont(font);
	  font = nullptr;
	  size = 0;
	}
}

// public non-member functions //
bool TT_Font_Init() {
	if (! Init) {
	  cout << "Initializing font support\n";
	  
	  Init = TTF_Init() == 0;
	  if (! Init)
	  	cerr << "Failed: " << TTF_GetError() << '\n';
	  else
	  	atexit(TT_Font_Quit);
	}
	
return Init;
}

void TT_Font_Quit() {
	if (Init) {
	  Init = false;
	  TTF_Quit();
	}
}

int TT_Font_LoadFont(Font& fontObj, const char* file, const int ptsize) {
	// if font api not initted
	if (! Init) {
	  cerr << INIT_ERROR << '\n';
	  return -1;
	}
	
	// invalid file argument
	if (file == nullptr) {
	  cerr << "TT_Font_LoadFont: file argument is NULL\n";
	  return -1;
	}
	
	// invalid font size
	if (ptsize <= 0) {
	  cerr << "TT_Font_LoadFont: the font size is zero or less, which is invalid\n";
	  return -1;
	}
	
	// font already loaded
	if (fontObj) {
	  cerr << "TT_Font_LoadFont: a font is already defined for the font object\n";
	  return -1;
	}
	
	// load font
	TTF_Font* tFont = TTF_OpenFont(file, ptsize);
	
	if (tFont == nullptr) {
	  cerr << "TT_Font_LoadFont: failed to load font -- " << TTF_GetError() << '\n';
	  return -1;
	}
	
	// define struct variables
	fontObj.font = tFont;
	fontObj.size = ptsize;
	
return 0;
}

int TT_Font_RenderText(Font& fontObj, const char* text, const SDL_Color& fgcolor, const SDL_Color* bgcolor) {
	// api not initted
	if (! Init) {
	  cerr << INIT_ERROR << '\n';
	  return -1;
	}
	
	// no font loaded
	if (! fontObj) {
	  cerr << "TT_Font_RenderText: no font has been defined\n";
	  return -1;
	}
	
	// invalid string
	if (text == nullptr) {
	  cerr << "TT_Font_RenderText: text argument is NULL\n";
	  return -1;
	}
	
	// replace old surface with a shiny new one
	fontObj.free();
	
	if (bgcolor != nullptr)
	  fontObj.surface = TTF_RenderText_Shaded(fontObj.font, text, fgcolor, *bgcolor);
	else
	  fontObj.surface = TTF_RenderText_Solid(fontObj.font, text, fgcolor);
	
	// failed to render surface
	if (fontObj.surface == nullptr) {
	  cerr << "failed to render text: " << TTF_GetError() << '\n';
	  return -1;
	}
	
return 0;
}

SDL_Color TT_Font_MapRGB(uint32_t color) {
	SDL_Color temp = {0};
	
	temp.r = (color & 0xff000000) >> 24;
	temp.g = (color & 0x00ff0000) >> 16;
	temp.b = (color & 0x0000ff00) >> 8;
	temp.a = color & 0x000000ff;
	
return move(temp);
}

// private functions //
