#include <cmath>
#include <ctime>

#include "camera.hpp"
#include "game.hpp"
#include "levelcode.hpp"
#include "levelinfo.hpp"
#include "math.hpp"
#include "memory.hpp"
#include "string.h"
#include "tmath.hpp"

#define cam_delta		0.13f
#define cam_delta2		0.20f
#define cam_spdx		1.5f
#define cam_spdx2		3.0f
#define cam_deacc		0.85f

enum CAMERA_SCROLL_DEF {
	CAMERA_NO_SCROLL,
	CAMERA_SCROLL_SLOW_DOWN,
	CAMERA_SCROLL_RIGHT,
	CAMERA_SCROLL_LEFT,
	CAMERA_SCROLL_FASTER_RIGHT,
	CAMERA_SCROLL_FASTER_LEFT,
};

StaticDArray<SDL_Rect, 3> camera::CamRects;
bool camera::Track = false;
float camera::CamXSpd = 0; // current x speed of the camera
float camera::CamYSpd = 0; // current y speed of the camera

// current behavior of the camera
static StaticDArray<_BackgroundLayer, 2> _Layers;
static CAMERA_SCROLL_DEF _Camera = CAMERA_NO_SCROLL;
static float _CamFrac[] = {0, 0};
static char _Offset = 0;

using namespace camera;
using level::ThePlayer;
using level::Header;

// returns true if the collision is inside the rectangle
INLINE bool pointInRect(const Point<int>& point, const SDL_Rect& clip) {
	return (point.x() >= clip.x) && (point.x() < (clip.x + clip.w)) &&
	(point.y() >= clip.y) && (point.y() < (clip.y + clip.h));
}

// scrolls the camera both directions until the sprite's center is visible
void camera::correctCamera() {
	PVector<int> diff;
	Point<int> sprite;
	
{
	// center of camera
	const Point<int> camera(CAM_CAMERA.x + CAM_CAMERA.w / 2, CAM_CAMERA.y + CAM_CAMERA.h / 2);
	
	// sprite center point
	const SDL_Rect& sprrect = ThePlayer->m_obj.get_dest_rect();
	sprite[0] = sprrect.x + sprrect.w / 2;
	sprite[1] = sprrect.y + sprrect.h / 2;
	
	// from camera to sprite (A->B)
	diff = sprite - camera;
}
	
	// scroll the camera to center on the player
	if (! pointInRect(sprite, CAM_CAMERA)) {
	  scrollx(diff.x());
	  scrolly(diff.y());
	}
}

void camera::defineLevel(const int width, const int height) {
	_BackgroundLayer temp;
	
  const int iSizes[] = {WIDTH, HEIGHT, cmul(WIDTH, 2), cmul(HEIGHT, 2), width, height};
	int x = 0;
	
  const float* fpMult = LevelInfo_BGScrollSpeeds(level::Header->background);
	
	// enable camera tracking if the level is bigger than the screen
	if (width > cdiv(WIDTH, 2) || height > cdiv(HEIGHT, 2))
	  Track = true;
	else
	  Track = false;
	
	// change all three rectangles
	for (auto itr = CamRects.begin(); itr != CamRects.end(); ++itr) {
	  itr->x = 0;
	  itr->y = 0;
	  itr->w = iSizes[x++];
	  itr->h = iSizes[x++];
	}
	
	// bottom bg layer
	String_memset(&temp, 0, sizeof(_BackgroundLayer));
	temp.xmult = fpMult[0];
	temp.ymult = fpMult[1];
	temp.rect.w = WIDTH;
	temp.rect.h = HEIGHT;
	_Layers[0] = temp;
	
	// top bg layer
	temp.xmult = fpMult[2];
	temp.ymult = fpMult[3];
	_Layers[1] = temp;
}

void camera::track() {
	// don't track the camera if disabled or player is at the beginning of the level
	if (! Track) return;
	
  const SDL_Rect& spriteLoc = ThePlayer->m_obj.get_dest_rect();
	SDL_Rect& cam       = CAM_CAMERA;
	
  const int iSprEdge = spriteLoc.x + spriteLoc.w;
	
	// check the player`s y position
	if (spriteLoc.y >= 128) {
	  // sprite center - camera center
	  // same as (spriteLoc.y + 34) > (cam.y + 320)
	  if ((spriteLoc.y - 286) > cam.y) {
	  	CamYSpd = LinearInterp(CamYSpd, 10, cam_delta2);
	  }
	  // same as (spriteLoc.y + 36) < (cam.y + 160)
	  else if ((spriteLoc.y - 126) < cam.y) {
	  	CamYSpd = LinearInterp(CamYSpd, -5, cam_delta2);
	  }
	  // no scroll
	  else {
	  	CamYSpd = Approach(CamYSpd, 0, 0.4f);
	  }
	}
	
	// camera can change direction when slowing down or stopped
	if (spriteLoc.x >= cdiv(WIDTH, 3) && (int) _Camera <= (int) CAMERA_SCROLL_SLOW_DOWN) {
	  if (iSprEdge > (cam.x + 427)) {
	  	_Camera = CAMERA_SCROLL_RIGHT;
	  }
	  else if (spriteLoc.x < (cam.x + 213)) {
	  	_Camera = CAMERA_SCROLL_LEFT;
	  	cam.x -= _Offset;
	  	_Offset = 0;
	  }
	}
	
	// act according to the camera scroll value
	switch (_Camera) {
	  default: break;
	  
	  // scrolls rightward at normal speed
	  case CAMERA_SCROLL_RIGHT: {
	  	// increase the current speed of the camera by *cam_delta*
	  	CamXSpd = LinearInterp(CamXSpd, cam_spdx, cam_delta);
	  	
	  	// if the speed exceeds the cap, decrease it by a certain percentage
	  	if (CamXSpd > cam_spdx)
	  	  CamXSpd *= cam_deacc;
	  	
	  	// if the player goes left of the 427th pixel in the camera, slow the camera down
	  	if (iSprEdge < (cam.x + 427))
	  	  _Camera = CAMERA_SCROLL_SLOW_DOWN;
	  	
	  	// if the player goes right of the camera's 491st pixel, scroll the camera faster
	  	if (iSprEdge > (cam.x + 491))
	  	  _Camera = CAMERA_SCROLL_FASTER_RIGHT;
	  	
//	  	scrollx(CAM_CAMERA, CamXSpd);
	  	break;
	  }
	  
	  // scrolls rightward at a higher speed
	  case CAMERA_SCROLL_FASTER_RIGHT: {
	  	// increase the current speed of the camera by a delta
	  	CamXSpd = LinearInterp(CamXSpd, cam_spdx2, cam_delta);
	  	
	  	// the player goes behing the 417th pixel in the camera region, revert back to normal speed
	  	if (iSprEdge < (cam.x + 417))
	  	  _Camera = CAMERA_SCROLL_RIGHT;
	  	
//	  	scrollx(CAM_CAMERA, CamXSpd);
	  	break;
	  }
	  
	  // scrolls leftward at the normal speed
	  case CAMERA_SCROLL_LEFT: {
	  	// increase the current speed of the camera by a delta
	  	CamXSpd = LinearInterp(CamXSpd, -cam_spdx, cam_delta);
	  	
	  	// if the player goes past the 213th pixel in the camera region, slow the camera down
	  	if (spriteLoc.x > (cam.x + 213))
	  	  _Camera = CAMERA_SCROLL_SLOW_DOWN;
	  	
	  	// if the player goes behind the camera region's 149th pixel, scroll the camera faster
	  	if (spriteLoc.x < (cam.x + 149))
	  	  _Camera = CAMERA_SCROLL_FASTER_LEFT;
	  	
//	  	scrollx(CAM_CAMERA, CamXSpd);
	  	break;
	  }
	  
	  // scrolls leftward at a higher speed
	  case CAMERA_SCROLL_FASTER_LEFT: {
	  	// increase the current speed of the camera by a delta
	  	CamXSpd = LinearInterp(CamXSpd, -cam_spdx2, cam_delta);
	  	
	  	// if the player goes past the 223rd pixel in the camera region, revert the camera back to its normal speed
	  	if (spriteLoc.x > (cam.x + 223))
	  	  _Camera = CAMERA_SCROLL_LEFT;
	  	
//	  	scrollx(CAM_CAMERA, CamXSpd);
	  	break;
	  }
	  
	  // the camera scrolls at an decreasing speeds until it finally stops
	  case CAMERA_SCROLL_SLOW_DOWN: {
	  	// reduce the camera's X speed by a multiplier
	  	CamXSpd *= cam_deacc;
	  	
	  	// if the absolute value of the X speed is close enough to zero, make it zero
	  	if (fabs(CamXSpd) < 0.21f)
	  	  CamXSpd = 0;
	  	
	  	// if the camera has no X speed, disable scrolling altogether
	  	if (! CamXSpd)
	  	  _Camera = CAMERA_NO_SCROLL;
	  	
//	  	scrollx(CAM_CAMERA, CamXSpd);
	  	break;
	  }
	}
	
	// scroll the camera along the x axis
	if (CamXSpd) {
	  scrollx(CamXSpd);
	  bgscrollx(_Layers[0], CamXSpd);
	}
}

void camera::scrollx(float speed) {
	float fCamx = _CamFrac[0];
	float fTemp;
	
	SDL_Rect& camera = CAM_CAMERA;
	
	// add speed to the fractional X speed and take the integer part out and add it to the camera's X position
	fCamx = std::modf(fCamx + speed, &fTemp);
	camera.x += (int) fTemp;
	
	// don't exceed level boundaries
	
	/* If the camera goes outside the level's left boundary, put it back at 0
	   (including its fraction value) and tell it to stop scrolling */
	if (camera.x < 0) {
	  camera.x = 0;
	  fCamx = 0;
	  CamXSpd = 0;
	  _Camera = CAMERA_NO_SCROLL;
	}
	
	/* check if the right edge of the camera has surpassed the level boundary (to its right)
	   if it did, offset the camera by that difference */
	fTemp = (camera.x + WIDTH) - CAM_LEVEL.w;
	
	if ((int) fTemp >= 0) {
	  camera.x = CAM_LEVEL.w - WIDTH;
	  _Offset = 1;
	  fCamx = 0;
	  CamXSpd = 0;
	  _Camera = CAMERA_NO_SCROLL;
	}
	
	_CamFrac[0] = fCamx;
}

void camera::bgscrollx(_BackgroundLayer& layer, float speed) {
	if (! speed || ! layer.xmult) return;
	
  const SDL_Rect& camera = CAM_CAMERA;
	float fTemp;
	
	layer.fx = std::modf(layer.fx + (speed * layer.xmult), &fTemp);
	layer.rect.x += fTemp;
	
	if (camera.x < layer.rect.x) {
	  layer.rect.x -= WIDTH;
	}
	else if (camera.x > (layer.rect.x + WIDTH)) {
	  layer.rect.x += WIDTH;
	}
}

void camera::scrolly(float speed) {
	float fCamy = _CamFrac[1];
	float fTemp;
	SDL_Rect& camera = CAM_CAMERA;
	
	// y speed
	fCamy += speed;
	fCamy = std::modf(fCamy, &fTemp);
	camera.y += fTemp;
	
	/* If the camera goes outside the level's top boundry, put it back at 0
	   (including its fraction value) and zero out its Y speed. */
	if (camera.y < 0) {
	  camera.y = fCamy = CamYSpd = 0;
	}
	
	/* check if the camera bottom edge has gone below the level boundary
	   if it did, then offset the camera by that difference */
	fTemp = (camera.y + csub(HEIGHT, 1)) - CAM_LEVEL.h;
	if ((int) fTemp >= 0) {
	  camera.y -= (int) fTemp;
	  fCamy = CamYSpd = 0;
	}
	
	_CamFrac[1] = fCamy;
}

const _BackgroundLayer* camera::getbglayers() {
	return _Layers.data();
}
