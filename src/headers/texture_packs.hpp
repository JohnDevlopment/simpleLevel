#ifndef TEXTURE_PACKS_HPP_INCLUDED
#define TEXTURE_PACKS_HPP_INCLUDED

#include <cstdio>
#include <iostream>
#include <custom/image_class.hpp>
#include <SDL.h>
#include <utility>

#include <custom/arrays.hpp>
#include "sprite_router.hpp"

#define NUMBER_OF_TEXTURE_PACKS	2

struct TexurePackArgs {
	std::string filename;   // file name
	uint8_t rgb[4];         // rgba array
	bool colorkey;          // enable colorkey
	SDL_Renderer* renderer; // renderer
	
	TexurePackArgs() : colorkey(false), renderer(nullptr) {
		*((uint32_t*) rgb) = 0U;
	}
	
	~TexurePackArgs() {
		filename.clear();
		renderer = nullptr;
	}
};

extern StaticDArray<TexturePack*, NUMBER_OF_UNIQUE_SPRITES> SpriteTXPackList;

// create a list of texture packs for each type of sprite
int NewTexturePacks(SDL_Renderer* renderer);

#endif /* TEXTURE_PACKS_HPP_INCLUDED */
