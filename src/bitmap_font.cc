#include "game.hpp"
#include "sdl_incs.h"
#include "bitmap_font.hpp"
#include "tt_fonts.hpp"
#include "log.hpp"

#include <utility>
#include <lvalue_rvalue_pointers.hpp>
#include <array>
#include <bad_option>
#include <fstream>
#include <cmath>

using namespace std;

//static void get_row_col(const int idx, uint8_t& row_r, uint8_t& col_r);

// class BitmapFont //
BitmapFont::BitmapFont() {
	memset((char*) this, 0, sizeof(BitmapFont));
}

// PRIVATE move data
void BitmapFont::move(BitmapFont&& src) {
	// transfer texture
	texture     = src.texture;
	src.texture = nullptr;
	
	// copy width to baseA
	memcpy(&width, &(src.width), sizeof(int) * 6);
	
	// copy a few global font metrics
	space	= src.space;
	newline	= src.newline;
	
	// copy fontChars
	memcpy(fontChars, &(src.fontChars), sizeof(SDL_Rect) * 256);
	
	// clear other object
	memset(&src, 0, sizeof(BitmapFont));
}

// free resources
void BitmapFont::free() {
	// free texture and reset pointer
	SDL_DestroyTexture(texture);
	texture = nullptr;
	
	// clear width to baseA
	memset(&width, 0, sizeof(int) * 6);
	space = 0;
	newline = 0;
	memset(&fontChars, 0, sizeof(SDL_Rect) * 256);
}

// build font
bool BitmapFont::build(string file) {
	SDL_Texture* ttex = texture;
	
	int tbak[] = {width, height};
	
	// offset of the top of the most elevated character
	top = 0;
	
	// bottom of the 'A' character
	baseA = height / 16;
	
	// open a channel to the binary file containing font data
	ifstream ifs (file.c_str(), ios::in | ios::binary);
	
	if (! ifs.is_open()) {
	  cerr << "BitmapFont::build: could not open " << file << '\n';
	  return false;
	}
	
	// seek to the end
	ifs.seekg(0, ios_base::end);
	
	// test the size of the file
	{
	  const size_t szBytes = (size_t) ifs.tellg();
	  
	  if (szBytes < sizeof(BitmapFont)) {
	  	cerr << "BitmapFont::build: file must be at least 4128 bytes large\n";
	  	ifs.close();
	  	return false;
	  }
	}
	
	// seek to the beginning
	ifs.seekg(0);
	
	// read the file into the structure
	ifs.read((char*) this, sizeof(BitmapFont));
	
	if (ifs)
	  Log_Cout("Successfully read from %s\n", file.c_str());
	else
	  Log_Cerr("Only managed to read %d bytes from %s\n", ifs.gcount(), file.c_str());
	
	// restore backed up values
	texture = ttex;
	width = tbak[0] > 0 ? tbak[0] : width;
	height = tbak[1] > 0 ? tbak[1] : height;
	
	// set width and height of each cell, representing the box of a character
	cellHeight = height / 16;
	cellWidth = width / 16;
	
	// space between each line
	newline = baseA - top;
	
	// space between characters
	space = cellWidth / 2;
	
	// close file handle
	ifs.close();
	
return true;
}


// load image
bool BitmapFont::open(SDL_Renderer* renderer, string file, uint32_t colorkey) {
	this->free(); // free own texture first
	
	SDL_Surface* tfmtsur = nullptr; // surface covnerted to texture later on
	
	// the red, green, and blue components of the color key
	array<uint8_t, 3> rgb;
	rgb[0] = RED_CHANNEL(colorkey);
	rgb[1] = GREEN_CHANNEL(colorkey);
	rgb[2] = BLUE_CHANNEL(colorkey);
	
	{
	  // load image onto surface
	  SDL_Surface* tsur = IMG_Load( file.c_str() );
	  
	  if (tsur == nullptr) {
	  	cerr << "BitmapFont::open: " << IMG_GetError() << '\n';
	  	return false;
	  }
	  
	  // convert surface to 16-bit color
	  tfmtsur = SDL_ConvertSurfaceFormat(tsur, SDL_PIXELFORMAT_RGBA4444, 0);
	  string err = SDL_GetError();
	  SDL_FreeSurface(tsur);
	  
	  if (tfmtsur == nullptr) {
	  	cerr << "BitmapFont::open: " << err << '\n';
	  	return false;
	  }
	}
	
	// reformat the color key value
	colorkey = SDL_MapRGB(tfmtsur->format, rgb[0], rgb[1], rgb[2]);
	
	// set the colorkey of the surface
	SDL_SetColorKey(tfmtsur, SDL_TRUE, colorkey);
	
	// convert the surface to a texture
	texture = SDL_CreateTextureFromSurface(renderer, tfmtsur);
	
	if (texture == nullptr) {
	  cerr << "BitmapFont::open: " << SDL_GetError() << '\n';
	  SDL_FreeSurface(tfmtsur);
	  return false;
	}
	
	// record size of the image
	this->width = tfmtsur->w;
	this->height = tfmtsur->h;
	
	// free surface
	SDL_FreeSurface(tfmtsur);
	
return true;
}


// load image
//bool BitmapFont::open(SDL_Renderer* renderer, string file, uint32_t colorkey) {
//	this->free(); // free own texture first
//	
//	SDL_Surface* tfmtsur = nullptr; // surface covnerted to texture later on
//	
//	// the red, green, and blue components of the color key
//	array<uint8_t, 3> rgb;
//	rgb[0] = RED_CHANNEL(colorkey);
//	rgb[1] = GREEN_CHANNEL(colorkey);
//	rgb[2] = BLUE_CHANNEL(colorkey);
//	
//	{
//	  // load image onto surface
//	  SDL_Surface* tsur = IMG_Load( file.c_str() );
//	  
//	  if (tsur == nullptr) {
//	  	cerr << "BitmapFont::open: " << IMG_GetError() << '\n';
//	  	return false;
//	  }
//	  
//	  // convert surface to 16-bit color
//	  tfmtsur = SDL_ConvertSurfaceFormat(tsur, SDL_PIXELFORMAT_RGBA4444, 0);
//	  string err = SDL_GetError();
//	  SDL_FreeSurface(tsur);
//	  
//	  if (tfmtsur == nullptr) {
//	  	cerr << "BitmapFont::open: " << err << '\n';
//	  	return false;
//	  }
//	}
//	
//	// reformat the color key value
//	colorkey = SDL_MapRGB(tfmtsur->format, rgb[0], rgb[1], rgb[2]);
//	
//	// convert the surface to a texture
//	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA4444, SDL_TEXTUREACCESS_STREAMING, tfmtsur->w, tfmtsur->h);
//	
//	if (texture == nullptr) {
//	  cerr << "BitmapFont::open: " << SDL_GetError() << '\n';
//	  SDL_FreeSurface(tfmtsur);
//	  return false;
//	}
//	
//	// array on the heap
//	uint32_t* tblk = new uint32_t[3];
//	
//	// array of texture pixels
//	uint16_t* uipPixels = nullptr;
//	
//	{
//	  // pointer to the texture's pixel data
//	  void* pixels = nullptr;
//	  
//	  // how many bytes there are per row of pixels
//	  int pitch;
//	  
//	  // enable alpha blending
//	  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
//	  
//	  // lock texture: pixels = array of pixel data, pitch = bytes per row of pixels
//	  SDL_LockTexture(texture, nullptr, &pixels, &pitch);
//	  
//	  // interpret array of 16-bit unsigned pixels
//	  uipPixels = (uint16_t*) pixels;
//	  
//	  // set values
//	  tblk[0] = static_cast<uint32_t>( (pitch >> 1) * tfmtsur->h ); // size of array (uipPixels)
//	  tblk[1] = SDL_MapRGBA(tfmtsur->format, RED_CHANNEL(colorkey), GREEN_CHANNEL(colorkey),
//	   BLUE_CHANNEL(colorkey), 0); // color value of a transparent pixel when the colorkey is detected
//	}
//	
//	// copy pixels from the surface to the texture
//	for (uint32_t i = 0; i < tblk[0]; ++i) {
//	  tblk[2] = reinterpret_cast<uint16_t*>(tfmtsur->pixels)[i]; // current pixel (surface)
//	  
//	  if (tblk[2] == colorkey)
//	  	uipPixels[i] = (uint16_t) tblk[1]; // copy transparent pixel
//	  else
//	  	uipPixels[i] = tblk[2]; // copy pixel as-is
//	}
//	
//	// apply changes to the texture
//	SDL_UnlockTexture(texture);
//	
//	// free array
//	delete[] tblk;
//	tblk = nullptr;
//	
//	// record size of the image
//	this->width = tfmtsur->w;
//	this->height = tfmtsur->h;
//	
//	// free surface
//	SDL_FreeSurface(tfmtsur);
//	
//return true;
//}

SDL_Point BitmapFont::cellNumber(const int idx) {
	const uint8_t size = 16;
	
	// location of the given cell in the table (units are cells, not pixels)
	SDL_Point ret;
	
	ret.y = idx / size;
	ret.x = idx - ret.y * size;
	
return std::move(ret);
}

void BitmapFont::render(SDL_Renderer* renderer, const string& str, int x, int y, int line) {
	if (texture == nullptr) return;
	
	int curx = x, cury = y, ch = 0;
	
	for (size_t i = 0; i < str.length(); ++i) {
	  switch (str[i]) {
	  	case ' ': {
	  	  curx += space;
	  	  break;
	  	}
	  	
	  	case '\n': {
	  	  cury += newline;
	  	  curx = x;
	  	  ch = 0;
	  	  break;
	  	}
	  	
	  	case '\t': {
	  	  curx += newline * 4;
	  	  break;
	  	}
	  	
	  	default: {
	  	  short int ch = (short int) str[i]; // current character
	  	  SDL_Rect src = fontChars[ch]; // clips the texture
	  	  
	  	  // location to blit character
	  	  SDL_Rect blit = {curx, cury, fontChars[ch].w, fontChars[ch].h};
	  	  
	  	  // index to coordinate (in cells, not pixels)
	  	  SDL_Point cell = cellNumber(ch);
	  	  
	  	  src.x += cell.x * cellWidth;
	  	  src.y += cell.y * cellHeight;
	  	  
	  	  // offset blit Y coordinate
	  	  blit.y += fontChars[ch].y;
	  	  
	  	  // render the character sprite
	  	  SDL_RenderCopy(renderer, texture, &src, &blit);
	  	  
	  	  // where the next character goes
	  	  curx += fontChars[ch].w + 1;
	  	  break;
	  	}
	  } // end switch
	  
	  // if characters on a line match $line, wrap to the next line
	  if (line > 0) {
	  	if (++ch == line) {
	  	  cury += newline;
	  	  curx = x;
	  	  ch = 0;
	  	}
	  }
	  
	} // end for
}
////////////////////////////////////////////////////
