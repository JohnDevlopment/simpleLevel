#ifndef _CAMERA_HPP
#define _CAMERA_HPP

// standard C/C++ library headers
#include "stdinc.h"

// SDL headers
#include "sdl_incs.h"

#include "background_images.hpp"

#include <vector>

#define CAM_CAMERA	camera::CameraLevel[0]
#define CAM_SCALED	camera::CameraLevel[1]
#define CAM_LEVEL	camera::CameraLevel[2]

namespace camera {
	extern std::vector<SDL_Rect> CameraLevel; // 0 = camera, 1 = level, 2 = backup of camera, 3 = backup of level
	extern bool Track;
	extern StaticDArray<Background_Base*,2> BGLayers;
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

inline void UnloadBackgroundObj() {
	using camera::BGLayers;
	for (size_t x = 0; x < BGLayers.size(); ++x) {
	  auto* ptr = BGLayers[x];
	  delete ptr;
	  BGLayers[x] = nullptr;
	}
}

inline void CopyCameraXY(SDL_Rect& dst, const SDL_Rect& src) {
	dst.x = src.x;
	dst.y = src.y;
}

#endif /* _CAMERA_HPP */
