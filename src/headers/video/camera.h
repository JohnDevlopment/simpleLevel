#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "stdinc.h"
#include "sdl_incs.h"
#include "global_attributes.hpp"

namespace video {
	// use the world camera
	void SetToWorldPerspective();
	
	// set the world camera
	void SetWorld(int w, int h);
	
	// set to HUD
	void SetToHUDPerspective();
	
	// set the HUD camera
	void SetHUD(int w, int h);
	
	// get a pointer to the game camera
	SDL_Rect* GetCamera();
	
	// set the active camera's position
	void SetCameraPos(int x, int y);
}

#endif /* CAMERA_H_INCLUDED */
