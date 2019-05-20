#ifndef EVENT_DEFINES_HPP
#define EVENT_DEFINES_HPP

#include <SDL.h>

// special defines
#define EVENT_USE_KEYSTATES	0x01

// event mask bits
#define EVENT_MASK_NOEVENTS	0x00
#define EVENT_MASK_KEYDOWN	0x01
#define EVENT_MASK_KEYUP	0x02
#define EVENT_MASK_MOUSEDOWN	0x04
#define EVENT_MASK_MOUSEUP	0x08
#define EVENT_MASK_MOUSEMOVE	0x10

// alternate names for SDL defines
#define MOUSE_BUTTON_LEFT	SDL_BUTTON_LEFT
#define MOUSE_BUTTON_MIDDLE	SDL_BUTTON_MIDDLE
#define MOUSE_BUTTON_RIGHT	SDL_BUTTON_RIGHT

#define EVENT_RELEASED		SDL_RELEASED
#define EVENT_PRESSED		SDL_PRESSED

// keyboard macros
#define sdl_keysym(event)	event.key.keysym.sym

// mouse macros
#define sdl_buttontype(event)	event.button.type
#define sdl_buttonstate(event)	event.button.state
#define sdl_twoclicks(event)	event.button.clicks == 2
#define sdl_oneclick(event)	event.button.clicks == 1
#define sdl_left(event)		event.button.button == SDL_BUTTON_LEFT
#define sdl_right(event)	event.button.button == SDL_BUTTON_RIGHT
#define sdl_wheel(event)	event.button.button == SDL_BUTTON_MIDDLE
#define sdl_leftbit(bitmask)	(bitmask & SDL_BUTTON(SDL_BUTTON_LEFT))
#define sdl_rightbit(bitmask)	(bitmask & SDL_BUTTON(SDL_BUTTON_RIGHT))
#define sdl_wheelbit(bitmask)	(bitmask & SDL_BUTTON(SDL_BUTTON_MIDDLE))

// event mask macros
#define MASKKEYS(mask)		mask & (EVENT_MASK_KEYDOWN|EVENT_MASK_KEYUP)
#define MASKBUTTONS(mask)	mask & (EVENT_MASK_MOUSEDOWN|EVENT_MASK_MOUSEUP)

#endif /* EVENT_DEFINES_HPP */
