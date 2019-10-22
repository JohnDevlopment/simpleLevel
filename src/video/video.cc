#include "video.h"

static SDL_Renderer* _private_renderer = nullptr;

void video::SetBgColor(const JColor& color) {
	SDL_SetRenderDrawColor(_private_renderer, color.red, color.green, color.blue, color.alpha);
}

void video::SetRenderer(SDL_Renderer* ren) {
	_private_renderer = ren;
}

SDL_Renderer* video::GetRenderer() {
	return _private_renderer;
}
