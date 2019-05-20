#include "headers/camera.hpp"
#include "headers/game.hpp"
#include "headers/levelcode.hpp"
#include "headers/math.hpp"
#include "headers/tmath.hpp"

#include <custom/lvalue_rvalue_pointers.hpp>
#include <cmath>

#define cam_delta	0.13f
#define cam_delta2	0.20f
#define cam_spdx	1.5f
#define cam_spdx2	3.0f
#define cam_deacc	0.85f

enum CAMERA_SCROLL_DEF {
	CAMERA_NO_SCROLL,
	CAMERA_SCROLL_RIGHT,
	CAMERA_SCROLL_LEFT,
	CAMERA_SCROLL_FASTER_RIGHT,
	CAMERA_SCROLL_FASTER_LEFT,
	CAMERA_SCROLL_SLOW_DOWN
};

std::vector<SDL_Rect> camera::CameraLevel;
bool camera::Track = false;
StaticDArray<Background_Base*,2> camera::BGLayers;
float camera::CamXSpd = 0; // current x speed of the camera
float camera::CamYSpd = 0; // current y speed of the camera
short int camera::BGMinX = 0;
short int camera::BGMinY = 0;

// fractional camera X/Y position
static float _Camx = 0;
static float _Camy = 0;

// current behavior of the camera
static CAMERA_SCROLL_DEF _Camera = CAMERA_NO_SCROLL;

using namespace camera;
using level::ThePlayer;
using level::Header;

// returns true if the collision is inside the rectangle
inline bool pointInRect(const Point<int>& point, const SDL_Rect& clip) {
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
	  camera::scrollx(diff.x());
	  camera::scrolly(diff.y());
	}
}

void camera::defineLevel(const int width, const int height) {
	// enable camera tracking if the level is bigger than the screen
	if (width > WIDTH/2 || height > HEIGHT/2)
	  Track = true;
	else
	  Track = false;
	
	// set the camera and level rects
	CameraLevel.push_back( SDL_Rect() );
	CAM_CAMERA.x = 0;
	CAM_CAMERA.y = 0;
	CAM_CAMERA.w = WIDTH;
	CAM_CAMERA.h = HEIGHT;
	
	CameraLevel.push_back( SDL_Rect() );
	CAM_SCALED.x = 0;
	CAM_SCALED.y = 0;
	CAM_SCALED.w = CAM_CAMERA.w * 2; 
	CAM_SCALED.h = CAM_CAMERA.h * 2;
	
	CameraLevel.push_back( SDL_Rect() );
	CAM_LEVEL.x = 0;
	CAM_LEVEL.y = 0;
	CAM_LEVEL.w = width;
	CAM_LEVEL.h = height;
	
	// minimum background offsets
	BGMinX = 0;
	BGMinY = 0;
}

void camera::track() {
	// camera rectangle
	SDL_Rect& cam = CAM_CAMERA;
	
	// don't track the camera if disabled or player is at the beginning of the level
	if (! Track) return;
	
	// track player's Y position and affect the camera accordingly
	if (ThePlayer->m_obj.y >= 128) {
	  int sprite_center = ThePlayer->m_obj.y + 32;
	  
	  // if player is above the 1/3 line of the camera
	  if (sprite_center < cam.y + 160)
	  	CamYSpd = LinearInterp(CamYSpd, -5, cam_delta2);
	  
	  // below the 2/3 line
	  else if (sprite_center > cam.y + 320)
	  	CamYSpd = LinearInterp(CamYSpd, 10, cam_delta2);
	  
	  // deaccelerate camera
	  else
	  	CamYSpd = CamYSpd > 0.2f ? CamYSpd * 0.3f : 0;
	  
	  if (CamYSpd) scrolly(CamYSpd);
	}
	
	// right edge of the player
	int sprite_right_edge = ThePlayer->m_obj.x + 32;
	
	// if camera is not moving or it is set to "slow down" mode, AND if the sprite is past a point
	if ((! _Camera || _Camera == CAMERA_SCROLL_SLOW_DOWN) && sprite_right_edge >= 320) {
	  // scroll right if the player goes past the 427'th pixel in the camera region
	  if (sprite_right_edge > (cam.x + 427))
	  	_Camera = CAMERA_SCROLL_RIGHT;
	  
	  // scroll left if the player goes before the 213'th pixel in the camera region
	  else if (ThePlayer->m_obj.x < (cam.x + 213))
	  	_Camera = CAMERA_SCROLL_LEFT;
	}
	
	// act according to the camera scroll value
	switch (_Camera) {
	  default: break;
	  
	  // scrolls rightward at normal speed
	  case CAMERA_SCROLL_RIGHT: {
	  	// increase the current speed of the camera by a delta
	  	CamXSpd = LinearInterp(CamXSpd, cam_spdx, cam_delta);
	  	
	  	// if the speed exceeds the cap, decrease it by a certain percentage
	  	if (CamXSpd > cam_spdx)
	  	  CamXSpd *= cam_deacc;
	  	
	  	// if the player goes left of the 427th pixel in the camera, slow the camera down
	  	if ( sprite_right_edge < (cam.x + 427) )
	  	  _Camera = CAMERA_SCROLL_SLOW_DOWN;
	  	
	  	// if the player goes right of the camera's 491st pixel, scroll the camera faster
	  	if ( sprite_right_edge > (cam.x + 491) )
	  	  _Camera = CAMERA_SCROLL_FASTER_RIGHT;
	  	
	  	// scroll the camera and background according to its current speed
	  	camera::scrollx(CamXSpd);
	  	break;
	  }
	  
	  // scrolls rightward at a higher speed
	  case CAMERA_SCROLL_FASTER_RIGHT: {
	  	// increase the current speed of the camera by a delta
	  	CamXSpd = LinearInterp(CamXSpd, cam_spdx2, cam_delta);
	  	
	  	// the player goes behing the 417th pixel in the camera region, revert back to normal speed
	  	if ( sprite_right_edge < (cam.x + 417) )
	  	  _Camera = CAMERA_SCROLL_RIGHT;
	  	
	  	// scroll the camera and background according to its current speed
	  	camera::scrollx(CamXSpd);
	  	break;
	  }
	  
	  // scrolls leftward at the normal speed
	  case CAMERA_SCROLL_LEFT: {
	  	// increase the current speed of the camera by a delta
	  	CamXSpd = LinearInterp(CamXSpd, -cam_spdx, cam_delta);
	  	
	  	// if the player goes past the 213th pixel in the camera region, slow the camera down
	  	if (ThePlayer->m_obj.x > (cam.x + 213))
	  	  _Camera = CAMERA_SCROLL_SLOW_DOWN;
	  	
	  	// if the player goes behind the camera region's 149th pixel, scroll the camera faster
	  	if (ThePlayer->m_obj.x < (cam.x + 149))
	  	  _Camera = CAMERA_SCROLL_FASTER_LEFT;
	  	
	  	// scroll the camera and background according to its current speed
	  	camera::scrollx(CamXSpd);
	  	break;
	  }
	  
	  // scrolls leftward at a higher speed
	  case CAMERA_SCROLL_FASTER_LEFT: {
	  	// increase the current speed of the camera by a delta
	  	CamXSpd = LinearInterp(CamXSpd, -cam_spdx2, cam_delta);
	  	
	  	// if the player goes past the 223rd pixel in the camera region, revert the camera back to its normal speed
	  	if (ThePlayer->m_obj.x > (cam.x + 223))
	  	  _Camera = CAMERA_SCROLL_LEFT;
	  	
	  	// scroll the camera and background according to its current speed
	  	camera::scrollx(CamXSpd);
	  	break;
	  }
	  
	  // the camera scrolls at an decreasing speeds until it finally stops
	  case CAMERA_SCROLL_SLOW_DOWN: {
	  	// reduce the camera's X speed by a multiplier
	  	CamXSpd *= cam_deacc;
	  	
	  	// scroll the camera and background according to its current speed
	  	camera::scrollx(CamXSpd);
	  	
	  	// if the absolute value of the X speed is close enough to zero, make it zero
	  	if (fabs(CamXSpd) < 0.21f)
	  	  CamXSpd = 0;
	  	
	  	// if the camera has no X speed, disable scrolling altogether
	  	if (! CamXSpd) _Camera = CAMERA_NO_SCROLL;
	  	
	  	break;
	  }
	}
	
	// scroll the background
	bgscroll(false);
}

void camera::scrolly(float speed) {
	SDL_Rect& camera = CAM_CAMERA;
	
	if (! speed) return;
	
	// y speed
	{
	  float temp;
	  _Camy += speed;
	  _Camy = std::modf(_Camy, &temp);
	  
	  camera.y += temp;
	}
	
	/* If the camera goes outside the level's top boundry, put it back at 0
	   (including its fraction value) and zero out its Y speed. */
	if (camera.y < 0) {
	  camera.y = 0;
	  _Camy = 0;
	  CamYSpd = 0;
	}
	
	// the camera goes below the bottom of the level
	{
	  int diff = bottom_edge(camera) - CAM_LEVEL.h;
	  if (diff >= 0) {
	  	camera.y -= diff + 1;
	  	_Camy = 0;
	  	CamYSpd = 0;
	  }
	}
}

void camera::scrollx(float speed) {
	SDL_Rect& camera = CAM_CAMERA;
	
	// if the argument is zero, replace it with CamXSpd
	if (! speed) return;
	
	// add speed to the fractional X speed and take the integer part out and add it to the camera's X position
	{
	  float temp;
	  
	  _Camx += speed;
	  _Camx = std::modf(_Camx, &temp);
	  
	  camera.x += temp;
	}
	
	// don't exceed level boundaries
	
	  /* If the camera goes outside the level's left boundary, put it back at 0
	     (including its fraction value) and tell it to stop scrolling */
	if (camera.x < 0) {
	  camera.x = 0;
	  _Camx = 0;
	  _Camera = CAMERA_NO_SCROLL;
	}
	
	  /* If the difference between the right edge of the camera and the level's width is >= 0, move the camera
	     left by that amount plus one. Furthermore, zero out the camera's fraction part and tell the camera
	     to stop scrolling. */
	{
	  int diff = right_edge(camera) - CAM_LEVEL.w;
	  
	  if (diff >= 0) {
	  	camera.x -= diff + 1;
	  	_Camx = 0;
	  	_Camera = CAMERA_NO_SCROLL;
	  }
	}
}

void camera::bgscroll(bool bypass) {
	/* If the camera is enabled, or if the bypass flag is active,
	   do the following. */
	if (_Camera != CAMERA_NO_SCROLL || bypass) {
	  Background_Base* ptr = nullptr;
	  int x = 0;
	  while (x < 2) {
	  	ptr = BGLayers[x++]; // increment x AFTER getting the pointer
	  	
	  	// if the pointer is not NULL
	  	if (ptr)
	  	  ptr->move();
	  }
	}
}

// class Background_X
void Background_X::move() {
	float fXspeed = *m_pfSpeedX * m_fFactorX;
	float temp;
	
	// add float x speed to float x offset, then separate integer and fraction parts
	m_fX += fXspeed;
	m_fX = std::modf(m_fX, &temp);
	m_iX += (int) temp; // this holds the integer part
}

void Background_X::check(const SDL_Rect* rect) {
//	if ( (rect->x - BGMinX) < (m_iX + BGMinX) )
//	  m_iX -= WIDTH;
//	else if ((rect->x + rect->w - BGMinX) > (m_iX + BGMinX + cmul(WIDTH, 2)))
//	  m_iX += WIDTH;
}

// class Background_Y
void Background_Y::move() {
	float fYspeed = *m_pfSpeedY * m_fFactorY;
	float temp;
	
	// add float y speed to float y offset, then separate integer and fraction parts
	m_fY += fYspeed;
	m_fY = std::modf(m_fY, &temp);
	m_iY += temp;
}

void Background_Y::check(const SDL_Rect* rect) {
	if (rect->y < m_iY)
	  m_iY += rect->y - m_iY;
	
	// above the level itself on top of the rect
	if (m_iY < 0 && rect->y > m_iY)
	  m_iY = 0;
}
