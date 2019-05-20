#ifndef GM_LEVEL_DEFS_H_INCLUDED
#define GM_LEVEL_DEFS_H_INCLUDED

// constants of gm_level's IO mode
typedef enum {
	GMLevel_Normal,
	GMLevel_Camera,
	GMLevel_Cutscene,
	GMLevel_Dialogue,
	GMLevel_Input
} GMLevel_IOMode;

// constants of indices to a list that sets the order of input processing
typedef enum {
	// key symbols
	GMLevel_Key_s,
	GMLevel_Key_Left,
	GMLevel_Key_Right,
	GMLevel_Key_Up,
	GMLevel_Key_l,
	GMLevel_Key_Escape,
	
	// indexes a buffer space in the queue list
	GMLevel_Key_Buffer,
	
	// number of evales
	GMLevel_Key_NumOfValues
} GMLevel_KeyListSym;

#endif /* GM_LEVEL_DEFS_H_INCLUDED */
