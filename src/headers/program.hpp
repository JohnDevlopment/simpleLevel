#ifndef _PROGRAM_H
#define _PROGRAM_H

#include <SDL.h>

struct PROGRAM {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* screen;
};

inline void destroyProgram(PROGRAM& program) {
	SDL_DestroyRenderer(program.renderer);
	SDL_DestroyWindow(program.window);
	program.window = NULL;
	program.renderer = NULL;
	program.screen = NULL;
}

#endif // _PROGRAM_H
