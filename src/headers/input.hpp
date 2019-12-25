#ifndef INPUT_HPP_INCLUDED
#define INPUT_HPP_INCLUDED

#include "stdinc.h"
#include "sdl_incs.h"

enum PDKeyCode {
	PDKey_Unknown = 0,
	
	PDKey_A, // 1
	PDKey_B,
	PDKey_C,
	PDKey_D,
	PDKey_E,
	PDKey_F,
	PDKey_G,
	PDKey_H,
	PDKey_I,
	PDKey_J,
	PDKey_K,
	PDKey_L,
	PDKey_M,
	PDKey_N,
	PDKey_O,
	PDKey_P,
	PDKey_Q,
	PDKey_R,
	PDKey_S,
	PDKey_T,
	PDKey_U,
	PDKey_V,
	PDKey_W,
	PDKey_X,
	PDKey_Y,
	PDKey_Z,
	
	PDKey_1, // 27
	PDKey_2,
	PDKey_3,
	PDKey_4,
	PDKey_5,
	PDKey_6,
	PDKey_7,
	PDKey_8,
	PDKey_9,
	PDKey_0,
	
	PDKey_Left, // 37
	PDKey_Right,
	PDKey_Up,
	PDKey_Down,
	
	PDKey_Last // 41
};

enum PDInputSym {
	PDSym_Unknown,
	PDSym_A,
	PDSym_B,
	PDSym_X,
	PDSym_Y
};

class PDInput {
	static void Init();
	static void Shutdown();
	static void Reset(float dt);
	static void Update(float dt);
//	static SDL_Scancode TranslateKey(PDKeyCode code);
	
	public:
	
	friend class PDApp;
	friend class PDGamemodeManager;
};

#endif /* INPUT_HPP_INCLUDED */
