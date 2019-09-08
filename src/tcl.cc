#include <cstdarg>

#include "camera.hpp"
#include "color.h"
#include "game.hpp"
#include "levelcode.hpp"
#include "levelinfo.hpp"
#include "log.hpp"
#include "memory.hpp"
#include "sound.hpp"
#include "sprite_router.hpp"
#include "stdinc.h"
#include "string.h"
#include "tcl.hpp"

#define TCL_CC 1
#define MAX_NUM_HASHES	2000

using namespace std;

typedef int (*TclCC_ProcSubFunc)(ClientData, Tcl_Interp*, int, Tcl_Obj* const*);

// public globals //
Tcl_Interp* gInterp = nullptr;

// private globals //

// private functions //
static int _private_GetHash(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	const char* sStr;
	int iStrLen, iMax, iOff;
	long int lRetval;
	
	if (objc != 4) {
	  Tcl_SetObjResult(gInterp, Tcl_NewStringObj("wrong # args: should be GetHash str max off", -1));
	  return TCL_ERROR;
	}
	
	// get string argument
	sStr = Tcl_GetStringFromObj(objv[1], &iStrLen);
	if (! sStr) {
	  Tcl_Obj* result = Tcl_ObjPrintf("error returned from GetHash: failed to get string argument: %s",
	                                 Tcl_GetStringResult(interp));
	  Tcl_SetObjResult(interp, result);
	  return TCL_ERROR;
	}
	
	// get second (int) argument
	if (Tcl_GetIntFromObj(interp, objv[2], &iMax) == TCL_ERROR) {
	  Tcl_Obj* result = Tcl_ObjPrintf("error returned from GetHash: %s not a valid integer field",
	                                  Tcl_GetStringFromObj(objv[2], nullptr));
	  Tcl_SetObjResult(interp, result);
	  return TCL_ERROR;
	}
	
	// get third (int) argument
	if (Tcl_GetIntFromObj(interp, objv[3], &iOff) == TCL_ERROR) {
	  Tcl_Obj* result = Tcl_ObjPrintf("error returned from GetHash: %s not a valid integer field",
	                                  Tcl_GetStringFromObj(objv[3], nullptr));
	  Tcl_SetObjResult(interp, result);
	  return TCL_ERROR;
	}
	
	// set return value for Tcl
	lRetval = static_cast<long int>(game::makeHash(sStr, iStrLen, iMax, iOff));
	Tcl_SetObjResult(interp, Tcl_NewLongObj(lRetval));
	
return TCL_OK;
}

static int _private_LevelBackground(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	int iAnInt;
	Tcl_Obj* result = nullptr;
	const char* sBuffer = nullptr;
	const _BackgroundLayer* layers = camera::getbglayers();
	
	if (! objc) {
	  TclCC_SetErrorResult(interp, "to few arguments to level background: should be include one of: color, test", -1);
	  return TCL_ERROR;
	}
	
	// check what the subcommand is
	sBuffer = Tcl_GetStringFromObj(objv[0], &iAnInt);
	switch (game::makeHash(sBuffer, iAnInt, MAX_NUM_HASHES, 300))
	{
	  default:
	  	result = Tcl_ObjPrintf("invalid subcommand \"%s\"", sBuffer ? sBuffer : "NULL");
	  	Tcl_SetObjResult(interp, result);
	  	iAnInt = -1;
	  	break;
	  
	  // "color"
	  case 0x000000000000022F:
	  	JColor color;
	  	
	  	if (objc != 4) {
	  	  TclCC_SetErrorResult(interp, "wrong # args: should be level background color red green blue", -1);
	  	  iAnInt = -1;
	  	  break;
	  	}
	  	
	  	// red channel
	  	if (Tcl_GetIntFromObj(interp, objv[1], &iAnInt) == TCL_ERROR) {
	  	  result = Tcl_ObjPrintf("failed to fetch integer argument 1: %s",
	  	                         Tcl_GetStringResult(interp));
	  	  iAnInt = -1;
	  	  break;
	  	}
	  	color.red = iAnInt;
	  	
	  	// green channel
	  	if (Tcl_GetIntFromObj(interp, objv[2], &iAnInt) == TCL_ERROR) {
	  	  result = Tcl_ObjPrintf("failed to fetch integer argument 2: %s",
	  	                         Tcl_GetStringResult(interp));
	  	  iAnInt = -1;
	  	  break;
	  	}
	  	color.green = iAnInt;
	  	
	  	// blue channel
	  	if (Tcl_GetIntFromObj(interp, objv[3], &iAnInt) == TCL_ERROR) {
	  	  result = Tcl_ObjPrintf("failed to fetch integer argument 3: %s",
	  	                         Tcl_GetStringResult(interp));
	  	  iAnInt = -1;
	  	  break;
	  	}
	  	color.blue = iAnInt;
	  	
	  	// change background color
	  	game::bgcolor(color);
	  	break;
	  
	  // "offsety"
	  case 0x00000000000005F9:
	  	if (objc != 3) {
	  	  TclCC_SetErrorResult(interp, "wrong # args: should be level background offsety bgnum value", 60);
	  	  iAnInt = -1;
	  	  break;
	  	}
	  	
	  	// bgnum
	  	if ( Tcl_GetIntFromObj(interp, objv[1], &iAnInt) == TCL_ERROR ) {
	  	  result = Tcl_ObjPrintf("failed to fetch integer argument 1: %s",
	  	                         Tcl_GetStringResult(interp));
	  	  iAnInt = -1;
	  	  break;
	  	}
	  	if (iAnInt) layers += iAnInt;
	  	
	  	// value
	  	if ( Tcl_GetIntFromObj(interp, objv[2], &iAnInt) == TCL_ERROR ) {
	  	  result = Tcl_ObjPrintf("failed to fetch integer argument 2: %s",
	  	                         Tcl_GetStringResult(interp));
	  	  iAnInt = -1;
	  	  break;
	  	}
	  	const_cast<_BackgroundLayer*>(layers)->rect.y += iAnInt;
	  	break;
	  
	  // "test"
	  case 0x00000000000001F6:
	  	cout << "test\n";
	  	break;
	}
	
	// return if an error result was produced
	if (iAnInt < 0) return TCL_ERROR;
	
return TCL_OK;
}

static int _private_LevelCommand(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	TclCC_ProcSubFunc subfunc = nullptr;
	const char* sArg;
	int iLen = 0;
	
	// subcommand
	sArg = Tcl_GetStringFromObj(objv[1], &iLen);
	if (! sArg || ! iLen) {
	  TclCC_SetErrorResult(interp, "level: invalid second argument", -1);
	  return TCL_ERROR;
	}
	
	// test the subcommand string`s hash against predefined values
	switch (game::makeHash(sArg, iLen, MAX_NUM_HASHES, 300)) {
	  default:
	  	//
	  	break;
	  
	  // "background"
	  case 0x00000000000002CF:
	  	subfunc = _private_LevelBackground;
	  	break;
	  
	  // "test"
	  case 0x00000000000005F7:
	  	cout << "subcommand \"test\"\n";
	  	break;
	}

return subfunc ? subfunc(cd, interp, objc-2, objv+2) : TCL_OK;
}

// public functions //
void TclCC_SetErrorResult(Tcl_Interp* interp, const char* str, int len) {
	if (len < 0)
	  len = String_strlen(str);
	
	Tcl_SetObjResult(interp, Tcl_NewStringObj(str, len));
}

int TclCC_AddToDict(Tcl_Interp* interp, Tcl_Obj** dict, const char* key, const char* val) {
	// key
	Tcl_Obj* keyObj = Tcl_NewStringObj(key, -1);
	if (keyObj == nullptr) return TCL_ERROR;
	
	// value
	Tcl_Obj* valObj = Tcl_NewStringObj(val, -1);
	if (valObj == nullptr) return TCL_ERROR;
	
	// append to dictionary
	if (Tcl_DictObjPut(interp, *dict, keyObj, valObj) == TCL_ERROR)
	  return TCL_ERROR;
	
return TCL_OK;
}

char* TclCC_AllocChar(size_t numBytes) {
	char* ptr_return = Tcl_Alloc(numBytes);

	if (ptr_return == nullptr)
	  throw bad_alloc ();

return ptr_return;
}

int TclCC_Init(PROGRAM* const program) {
	using level::CurrentLevel;

	char sName[31];
	
	Log_Cout("### Initializing Tcl ###\n");

	// create Tcl interpreter
	gInterp = Tcl_CreateInterp();

	if (gInterp == nullptr) {
	  cerr << "Failed to create interpreter for Tcl\n";
	  return TCL_ERROR;
	}

	// initialize Tcl
	if (Tcl_Init(gInterp) != TCL_OK) {
	  cerr << "Unable to initialize Tcl\n";
	  return TCL_ERROR;
	}
	
	// link CurrentLevel to Tcl variable
	if (Tcl_LinkVar(gInterp, "CurrentLevel", (char*) &CurrentLevel, TCL_LINK_STRING) == TCL_ERROR) {
	  cerr << "Failed to link CurrentLevel\n";
	  return TCL_ERROR;
	}
	
	// link GetHash to _private_GetHash
	String_strcpy(sName, "GetHash");
	if (! Tcl_CreateObjCommand(gInterp, sName, _private_GetHash, nullptr, nullptr)) {
	  cerr << "Failed to link GetHash: " << Tcl_GetStringResult(gInterp) << '\n';
	  return TCL_ERROR;
	}
	
	// link "level" to _private_LevelCommand
	String_strcpy(sName, "level");
	if (! Tcl_CreateObjCommand(gInterp, sName, _private_LevelCommand, nullptr, nullptr)) {
	  cerr << "Failed to link level command: " << Tcl_GetStringResult(gInterp) << '\n';
	  return TCL_ERROR;
	}
	
	// include script that defines globals
	if (Tcl_Eval(gInterp, "set SIMPLELEVEL 1; source scripts/main.tcl") == TCL_ERROR) {
	  cerr << Tcl_GetStringResult(gInterp) << '\n';
	  return TCL_ERROR;
	}
	
	Log_Cout("Tcl variable CurrentLevel linked to C variable\nTcl initialized\n\n");
	
return TCL_OK;
}

void TclCC_Quit() {
	Tcl_UnlinkVar(gInterp, "CurrentLevel");
	Tcl_Free(level::CurrentLevel);
	Tcl_DeleteInterp(gInterp);
}
