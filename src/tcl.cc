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

using namespace std;

Tcl_Interp* gInterp = nullptr;

// private functions //
static int _private_GetHash(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	const char* sStr;
	int iStrLen;
	long int lRetval;
	
	if (objc != 2) {
	  Tcl_SetObjResult(gInterp, Tcl_NewStringObj("wrong # args: should be GetHash str", -1));
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
	
	lRetval = static_cast<long int>(game::makeHash(sStr, iStrLen, 5000));
	Tcl_SetObjResult(interp, Tcl_NewLongObj(lRetval));
	
return TCL_OK;
}

static int _private_LevelCommand(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {



return TCL_OK;
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
	if (Tcl_Eval(gInterp, "set SIMPLELEVEL 1; source scripts/globals.tcl") == TCL_ERROR) {
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
