#ifndef BACKGROUND_IMAGES_HPP_INCLUDED
#define BACKGROUND_IMAGES_HPP_INCLUDED

#include "stdinc.h"
#include "sdl_incs.h"

#include <custom/pointvector>
#include <iostream>
#include <custom/image_class.hpp>

enum {
	BG_X = 1,
	BG_Y,
	BG_XY
};

// background base class; defines the basic structure of a background class
class Background_Base {
public:
	Background_Base() {}
	virtual ~Background_Base() {}
	
	virtual void clear() = 0;
	virtual int get_type() const = 0;
	virtual void move() = 0;
	virtual Point<int> get_position() const = 0;
	virtual int& x() = 0;
	virtual int& y() = 0;
	virtual void check(const SDL_Rect*) = 0;
	virtual void set(int, int, float, float) = 0;
};

// derived background class that's specialized for the X axis
class Background_X : virtual public Background_Base {
protected:
	int m_iX;
	float m_fX;
	float m_fFactorX;
	float* m_pfSpeedX;
	
public:
	Background_X(float factor, float* xspd_ptr) : m_iX(0),
			m_fX(0), m_fFactorX(factor), m_pfSpeedX(xspd_ptr) {}
	
	virtual ~Background_X() {}
	
	// clears internal data
	virtual void clear() {
		m_iX = 0;
		m_fX = 0;
	}
	
	// integer of what scrolling this background layer supports
	virtual int get_type() const {
		return BG_X;
	}
	
	// scroll the background
	virtual void move();
	
	// returns the position
	virtual Point<int> get_position() const {
		return Point<int>(m_iX, 0);
	}
	
	// checks if the provided rectangle is within bounds of the background layer
	virtual void check(const SDL_Rect* rect);
	
	int& x() {return m_iX;}
	int& y() {return m_iX;}
	
	// sets position and speed factor of the background
	void set(int x, int y, float factx, float facty) {
		m_iX = x;
		m_fFactorX = factx;
	}
};

// derived background class that's specialized for the Y axis
class Background_Y : virtual public Background_Base {
protected:
	int m_iY;
	float m_fY;
	float m_fFactorY;
	float* m_pfSpeedY;
	
public:
	Background_Y(float factor, float* yspd_ptr) : m_iY(0), m_fY(0), m_fFactorY(factor), m_pfSpeedY(yspd_ptr) {}
	virtual ~Background_Y() {}
	
	// clears internal data
	virtual void clear() {
		m_iY = 0;
		m_fY = 0;
	}
	
	// returns background type
	virtual int get_type() const {
		return BG_Y;
	}
	
	// scroll the background
	virtual void move();
	
	// returns the position
	virtual Point<int> get_position() const {
		return Point<int>(0, m_iY);
	}
	
	// ...
	virtual void check(const SDL_Rect* rect);
	
	int& x() {return m_iY;}
	int& y() {return m_iY;}
	
	// sets position and speed factor of the background
	void set(int x, int y, float factx, float facty) {
		m_iY = y;
		m_fFactorY = facty;
	}
};

// derived background class that does both the X and Y axis
class Background_XY : public Background_X, public Background_Y {
public:
	Background_XY(float factx, float facty, float* xspd_ptr, float* yspd_ptr)
		: Background_X(factx, xspd_ptr), Background_Y(facty, yspd_ptr) {}
	~Background_XY() {}
	
	// clears internal data
	void clear() {
		Background_X::clear();
		Background_Y::clear();
	}
	
	// returns background type
	int get_type() const {
		return BG_XY;
	}
	
	// scroll the background
	void move() {
		Background_X::move();
		Background_Y::move();
	}
	
	// returns the position
	Point<int> get_position() const {
		return Point<int>(m_iX, m_iY);
	}
	
	// ...
	void check(const SDL_Rect* rect) {
		Background_X::check(rect);
		Background_Y::check(rect);
	}
	
	int& x() {return m_iX;}
	int& y() {return m_iY;}
	
	// sets position and speed factor of the background
	void set(int x, int y, float factx, float facty) {
		m_iX = x;
		m_iY = y;
		m_fFactorX = factx;
		m_fFactorY = facty;
	}
};

#endif /* BACKGROUND_IMAGES_HPP_INCLUDED */
