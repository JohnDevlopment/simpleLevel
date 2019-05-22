#ifndef GAME_HPP
#define GAME_HPP

// holds one instance of an object on the heap; good for large objects
#include <custom/generic_class.hpp>

// for loading and displaying images in an easier manner
#include <custom/image_class.hpp>

// for manipulating bits in an integer
#include <custom/bitfield_class.hpp>

// for loading level .dat files
#include <custom/levelLoad.hpp>

// std::vector -- good for ­really large arrays
#include <vector>

// standard headers
#include "stdinc.h"

// vectors and points
#include <custom/pointvector>

// fonts
#include "tt_fonts.hpp"

// affects how functions are compiled
#include "global_attributes.hpp"

// program window and renderer
#include "program.hpp"

// color struct
#include "color.h"

// SDL files
#include "sdl_incs.h"

#define STRLEN(i)		std::strlen(#i)

// individual flags
#define TEXTURE_PACKS		0x0200
#define LEVEL_CLEANUP		0x0100
#define FADEOUT			0x0080
#define FADEIN			0x0040
#define SOUND_API_INIT		0x0020
#define CAMERA_SCROLL		0x0010
#define TRIGGERS_API_INIT	0x0008
#define QUITGAME		0x0004
#define DISABLE_INPUT		0x0002
#define RES_FILE		0x0001

// composite flags
#define FADING			(FADEOUT | FADEIN)

// parts of the array game::Graphics
#define BG_BLACK		game::Graphics[0]
#define BG_BG1			game::Graphics[1]
#define BG_BG2			game::Graphics[2]
#define BG_DIALOGUEBOX		game::Graphics[3]
#define BG_BACKUP1		game::Graphics[4]
#define BG_BACKUP2		game::Graphics[5]
#define NUM_IMAGES		6
#define NUM_BGS			2

// other macros
#define right_edge(rect)	(rect.x + rect.w)
#define bottom_edge(rect)	(rect.y + rect.h)
#define PRINT_VAR(variable)	std::cout << #variable << " = " << variable << std::endl

/* Namespace for variables that should be accessible to every source file in the program
   at the global scale.
   
   Flags: bit 10 (0x0000 or 0000 0100 0000 0000) = undefined
          bit  9 (0x0200 or 0000 0010 0000 0000) = texture packs have been loaded if this is set
          bit  8 (0x0100 or 0000 0001 0000 0000) = cleanup certain blocks of memory at the gm_level cleanup routine
          bit  7 (0x0080 or 0000 0000 1000 0000) = fade to black
          bit  6 (0x0040 or 0000 0000 0100 0000) = fade from black
          bit  5 (0x0020 or 0000 0000 0010 0000) = music system initialized
          bit  4 (0x0010 or 0000 0000 0001 0000) = level scroll enabled
          bit  3 (0x0008 or 0000 0000 0000 1000) = trigger system activated
          bit  2 (0x0004 or 0000 0000 0000 0100) = quit game (gm_level)
          bit  1 (0x0002 or 0000 0000 0000 0010) = disable input when set
          bit  0 (0x0001 or 0000 0000 0000 0001) = indicates DLOpen has been called and the resource file is loaded
*/
namespace game {
	// complex object types
	extern std::vector<Image>	 Graphics;
	extern generic_class<BitmapFont> Bitmap;
	extern Bitfield<uint16_t>	 Flags;
	
	// integer types
	extern uint8_t			 FrameCounter;
	extern uint32_t			 MiscTimer;
	extern uint32_t*		 SaveData; // dynamic array: currently holds 3
	
	void unload_backgrounds();
	
	// loads image files, fonts, and allocates save data
	bool loadMedia(SDL_Renderer*);
	
	// frees the game's resources
	void free();
	
	// change background color
	void bgcolor(const JColor& color);
}

template<typename T, typename... Args>
void Free(T** dptr, Args&&... args) {
	Free(dptr);
	Free(std::forward<Args>(args)...);
}

template<>
inline void Free<SDL_Surface>(SDL_Surface** surface) {
	if (surface != nullptr) {
	  SDL_FreeSurface(*surface);
	  *surface = nullptr;
	}
}

template<>
inline void Free<SDL_Texture>(SDL_Texture** texture) {
	if (texture != nullptr) {
	  SDL_DestroyTexture(*texture);
	  *texture = nullptr;
	}
}

// insert a point's X & Y values into SDL_Rect
Point<int>& operator>>(Point<int>&, SDL_Rect& rect);

// insert a rectangle's X & Y values into Point<int>
Point<int>& operator<<(Point<int>& point, SDL_Rect& rect);

#endif /* GAME_HPP */