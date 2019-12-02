#include "camera.h"

namespace {
	SDL_Rect* _Region = nullptr;
	SDL_Rect _Drawspaces[2]; // world & HUD
}

namespace video {

void SetToWorldPerspective() {
	_Region = &_Drawspaces[0];
}

void SetWorld(int w, int h) {
	SDL_Rect* pTemp = &_Drawspaces[0];
	pTemp->x = 0;
	pTemp->y = 0;
	pTemp->w = w;
	pTemp->h = h;
}

void SetToHUDPerspective() {
	_Region = &_Drawspaces[1];
}

void SetHUD(int w, int h) {
	SDL_Rect* pTemp = &_Drawspaces[1];
	pTemp->x = 0;
	pTemp->y = 0;
	pTemp->w = w;
	pTemp->h = h;
}

SDL_Rect* GetCamera() {
	return _Region;
}

void SetCameraPos(int x, int y) {
	_Region->x = x;
	_Region->y = y;
}

}
