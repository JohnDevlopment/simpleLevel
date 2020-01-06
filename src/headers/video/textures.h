#ifndef TEXTURES_H_INCLUDED
#define TEXTURES_H_INCLUDED

#include "video.h"
#include "log.hpp"
#include "pointvector"
#include "string.h"
#include "patternarray.h"

namespace video {
	/* Loads a texture from file with an option colorkey. If the first function is
	   called with a NULL file string, but the renderer isn't, that renderer gets remembered
	   and the second function can be called later on. */
	SDL_Texture* LoadTexture(SDL_Renderer* ren, const char* file, const uint32_t* colorkey = nullptr);
	SDL_Texture* LoadTexture(const char* file, const uint32_t* colorkey = nullptr);
	
	/*  */
	SDL_Texture* NewColorScreen(const uint8_t* color, int width, int height, uint32_t format, PatternArray& pattern);
}

#endif /* TEXTURES_H_INCLUDED */
