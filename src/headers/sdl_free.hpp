#ifndef _FREE_H
#define _FREE_H

#include <SDL2/SDL.h>
#include <utility>

template<typename T, typename... Args>
void Free(T* _t, Args&&... _args) {
	Free(_t);
	Free( std::forward<Args>(_args)... );
}

template<>
inline void Free<SDL_Surface>(SDL_Surface* _sur) {
	if (_sur != NULL)
	  SDL_FreeSurface(_sur);
}

template<>
inline void Free<SDL_Renderer>(SDL_Renderer* _ren) {
	if (_ren != NULL)
	  SDL_DestroyRenderer(_ren);
}

template<>
inline void Free<SDL_Window>(SDL_Window* _win) {
	if (_win != NULL)
	  SDL_DestroyWindow(_win);
}

template<>
inline void Free<SDL_Texture>(SDL_Texture* _tex) {
	if (_tex != NULL)
	  SDL_DestroyTexture(_tex);
}

#endif // _FREE_H
