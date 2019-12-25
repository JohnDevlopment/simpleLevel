#ifndef GAME_HPP
#define GAME_HPP

// engine headers
#include "stdinc.h"
#include "sdl_incs.h"
#include "global_attributes.hpp"
#include "program.hpp"
#include "textures.h"
#include "pdresourcemanager.hpp"
#include "registerpdcommandtypes.h"

// other headers
#include "string.h"
#include "generic_class.hpp"
#include "bitfield_class.hpp"
#include "levelLoad.hpp"
#include "pointvector"
#include "color.h"

extern PDResourceManager RManager;

/**************************************************/
/*
Shared game data.
*/
/**************************************************/
struct PDGameData {
	unsigned int fps;
	unsigned int ticks;
	float sec;
	unsigned int width;
	unsigned int height;
};

// free a variable list of objects from the heap
template<typename T, typename... Args>
void Free(T* ptr, Args&&... args) {
	Free(ptr);
	Free(std::forward<Args>(args)...);
}

template<>
inline void Free<SDL_Surface>(SDL_Surface* surface) {
	if (surface)
	  SDL_FreeSurface(surface);
}

template<>
inline void Free<SDL_Texture>(SDL_Texture* texture) {
	if (texture)
	  SDL_DestroyTexture(texture);
}

#endif /* GAME_HPP */
