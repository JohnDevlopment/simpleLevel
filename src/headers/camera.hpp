#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include "stdinc.h"
#include "sdl_incs.h"
#include <vector>

#define CAM_CAMERA	camera::CamRects[0]
#define CAM_SCALED	camera::CamRects[1]
#define CAM_LEVEL	camera::CamRects[2]

namespace camera {
	extern StaticDArray<SDL_Rect, 4> CamRects;
	extern bool Track;
	extern float CamXSpd;
	extern float CamYSpd;
	extern short int BGMinX;
	extern short int BGMinY;
	
	void defineLevel(const int width, const int height);
	void track();
	void scrollx(float speed);
	void scrolly(float speed);
	void bgscroll(bool bypass);
	void correctCamera();
}

#endif /* _CAMERA_HPP */
