#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include "stdinc.h"
#include "sdl_incs.h"
#include "global_attributes.hpp"

#include <vector>

#define CAM_CAMERA	camera::CamRects[0]
#define CAM_SCALED	camera::CamRects[1]
#define CAM_LEVEL	camera::CamRects[2]

struct _BackgroundLayer {
	SDL_Rect rect;
	float fx;
	float fy;
	float xmult;
	float ymult;
	int result;
};

namespace camera {
	extern StaticDArray<SDL_Rect, 3> CamRects;
	extern bool Track;
	extern float CamXSpd;
	extern float CamYSpd;
	
	void defineLevel(const int width, const int height);
	void track();
	void scrollx(float speed);
	void bgscrollx(_BackgroundLayer& layer, float speed);
	void scrolly(float speed);
	void bgscrolly(_BackgroundLayer& layer, float speed);
	void correctCamera();
	const _BackgroundLayer* getbglayers();
}

#endif /* _CAMERA_HPP */
