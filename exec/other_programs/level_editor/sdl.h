#ifndef SDL_H_INCLUDED
#define SDL_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <utility>

#include "program.h"

// initializes SDL; returns zero if successful
bool initsdl(Program& pg);

// returns a SDL_Rect with the given values
SDL_Rect makerect(int x, int y, int w, int h);

// free resources
template<typename T, typename... Args>
void SDL_Free(T* first, Args&&... args)
{
	SDL_Free(first);
	SDL_Free( std::forward<Args>(args)... );
}

template<>
inline void SDL_Free<SDL_Texture>(SDL_Texture* tex)
{
	if (tex)
	  SDL_DestroyTexture(tex);
}

template<>
inline void SDL_Free<SDL_Window>(SDL_Window* win)
{
	if (win)
	  SDL_DestroyWindow(win);
}

template<>
inline void SDL_Free<SDL_Renderer>(SDL_Renderer* ren)
{
	if (ren)
	  SDL_DestroyRenderer(ren);
}

#endif // SDL_H_INCLUDED
