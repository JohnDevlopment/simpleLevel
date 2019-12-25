#ifndef OBJECT_CLASS_HPP
#define OBJECT_CLASS_HPP

#include <custom/image_class.hpp>
#include <custom/bits/object_class_defines.hpp>
#include <custom/bits/object_class_coord.hpp>
#include <custom/bad_option>
#include <utility>
#include <cstring>
#include <new>

#ifdef DEBUG
#  include <cstdio>
#endif

// first argument: pointer to an integer array, two elements (X & Y of the object)
// second argument: pointer to a float array, six elements (deltax, deltay, cspeedx, cspeedy, gspeedx, gspeedy)
// third argument: constant lvalue-reference to the collision hitbox
typedef void (* CollisionFunction)(int*, float*, const SDL_Rect&);

class Object : public Image {
	float Deltax;
	float Deltay;
	float CSpeedx;
	float CSpeedy;
	float GSpeedx;
	float GSpeedy;
	float FracX;
	float FracY;

	SDL_Rect CollisionRect;
	CollisionFunction PFunction;

	float GravityDelta;
	float GravitySpeed;
	bool  FlipGravity;

	size_t NumObjects;

	// sets default values to data members
	void set_vals();

	// get the XY offsets of the current frame
	void clip_frame();

	// disable copy
	Object& operator=(const Object& _rhs);

	// transfer values between objects, clearing the source
	void move(Object&& src);

	public:

	// frame to clip
	uint16_t Frame;

	// object's X & Y position, can be read from and written to
	COORD x;
	COORD y;

	// calculates speed at a constant rate using a delta
	static float approach(float current, float goal, const float delta);

	Object() : GravityDelta(0), PFunction(nullptr), NumObjects(0), Frame(0) {
		set_vals();
	}

	Object(size_t _num) : GravityDelta(0), PFunction(nullptr), NumObjects(_num), Frame(0) {
		set_vals();
	}

	Object(Object&& _src) {
		set_vals();
		this->move( std::move(_src) );
	}

	~Object() {}

	// overload new and delete //

	// allocates memory for one instance of this class
	static void* operator new(size_t num);

	// frees memory that was previously allocated using new
	static void operator delete(void* ptr);

	// allocates memory for *num* instances of this class in continguous memory
	static void* operator new[](size_t num);

	// frees memory that was previously allocated using new[]
	static void operator delete[](void* ptr);


	// assignment functions //

	/* All three functions have to do with setting the X speed of the object. The first one
	   sets the target velocity of the object, which means the object will accelerate toward
	   that goal. The second function immendiately changes the speed of the object. And the third
	   function does what the first two functions did, but all at once.
	*/
	void set_x_vel(float _x) {
		GSpeedx = _x;
		FracX = (! _x) ? 0 : FracX;
	}
	void set_x_speed(float _x) {
		CSpeedx = _x;
	}
	void set_x_speed_and_vel(float _x, float _gx) {
		CSpeedx =  _x;
		GSpeedx = _gx;
		FracX = (! _x) ? 0 : FracX;
	}

	/* All three functions have to do with setting the Y speed of the object. The first one
	   sets the target velocity of the object, which means the object will accelerate toward
	   that goal. The second function immediately changes the speed of the object. And the third
	   function does what the first two functions did, but all at once.
	*/
	void set_y_vel(float _y) {
		GSpeedy = _y;
		FracY = (_y == 0) ? 0 : FracY;
	}
	void set_y_speed(float _y) {
		CSpeedy = _y;
	}
	void set_y_speed_and_vel(float _y, float _gy) {
		CSpeedy = _y;
		GSpeedy = _gy;
		FracY = (_y == 0) ? 0 : FracY;
	}

	/* Both functions set the delta factor for horizontal speeds. The first form
	   simply assigns the value provided by the caller to the delta variable. But the
	   second form accepts the active framerate and an acceleration value, to calculate
	   the final result itself. That form is the most covenient if you don't know what
	   value to put in.
	*/
	void set_x_delta(float _dt) {
		Deltax = _dt;
	}
	void set_x_delta(float _fps, float _acc) {
		Deltax = calc_delta(_fps, _acc);
	}

	/* Both functions set the delta factor for vertical speeds. The first form accepts
	   a value that's directly assigned to the appropriate variable. But the second form
	   takes a framerate and an acceleration value, and calculates the delta factor based
	   off of that. The second form is most convenient to use most of the time.
	*/
	void set_y_delta(float _dt) {
		Deltay = _dt;
	}
	void set_y_delta(float _fps, float _acc) {
		Deltay = calc_delta(_fps, _acc);
	}

	/* Both functions set the delta factor for gravity. The way gravity works in this class
	   if that it affects both the current speed and the target speed at once. See the documentation
	   on the move() function to see exactly how speed works in this library. Suffice it to say,
	   the target Y velocity will gradually
	*/
	void set_gravity_delta(float _dt) {
		GravityDelta = (_dt > 0) ? _dt : 0;
	}
	void set_gravity_delta(float _fps, float _acc) {
		GravityDelta = calc_delta(_fps, _acc);
	}

	// set the speed cap for gravity
	void set_gravity_speed(float _spd) {
		GravitySpeed = _spd;
	}

	// registers a function to PFunction
	void set_collision_function(CollisionFunction pfunc = nullptr) {
		PFunction = pfunc;
	}

	// set hitbox
	void set_hitbox(int _x, int _y, int _w, int _h) {
		CollisionRect.x = _x;
		CollisionRect.y = _y;
		CollisionRect.w = _w;
		CollisionRect.h = _h;
	}

	// set the hitbox to the rectangle parameter
	void set_hitbox(const SDL_Rect& _rect) {
		CollisionRect.x = _rect.x;
		CollisionRect.y = _rect.y;
		CollisionRect.w = _rect.w;
		CollisionRect.h = _rect.h;
	}


	// accessors //

	// returns the x delta factor
	float get_x_delta() const {return Deltax;}

	// returns the y delta factor
	float get_y_delta() const {return Deltay;}

	// get the current X speed
	float get_x_speed() const {return CSpeedx;}

	// get the current Y speed
	float get_y_speed() const {return CSpeedy;}

	// get the target X speed
	float get_x_vel() const {return GSpeedx;}

	// get the target Y speed
	float get_y_vel() const {return GSpeedy;}

	// get hitbox
	SDL_Rect& get_hitbox() {return CollisionRect;}


	// misc functions //

	/* Updates the sprite's x & y position according to speed.
	   The x speed and the y speed are calculated using their own
	   user-defined deltas. Gravity is also calculated if its corresponding
	   delta is set.
	   
	   In order for gravity to work, two variables must be set: GravityDelta, the
	   value that the speed increases each frame; and GravitySpeed, the final velocity
	   for gravity. The goal Y speed will gradate to this value, which in turn will
	   affect the current Y speed.
	   
	   Calls a function to do modifications to the sprite's position and speed
	   in an external function pointed to by PFunction. If PFunction isn't set,
	   the function won't be called. PFunction must accept three arguments: first
	   a pointer to int, which points to an array of four elements, which are the X
	   and Y offset of the sprite, and its blitting width and height; second, a pointer
	   to float, which points to a six-element array that denotes the current X & Y speed,
	   goal X & Y speed, and the X & Y deltas of the sprite; and third, a constant reference
	   to SDL_Rect representing the the sprite's hitbox. The function is generally called
	   to do collision detection for the sprite.
	*/
	void move();

	// blits the object to the screen using its internal X and Y values
	virtual void blit(const SDL_Rect* vpt = nullptr);

	// calulcates a delta value based on the framerate and an acceleration value
	static float calc_delta(const float fps, const float acceleration) {
		return (1.f / fps) * (acceleration + 1);
	}
};

#endif /* OBJECT_CLASS_HPP */
