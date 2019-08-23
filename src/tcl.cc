// standard includes
#include "headers/stdinc.h"

#include "sound.hpp"
#include "tcl.hpp"
#include "sprite_router.hpp"
#include "levelcode.hpp"
#include "triggers.hpp"
#include "log.hpp"
#include "background_images.hpp"
#include "camera.hpp"
#include "levelinfo.hpp"
#include "color.h"
#include "memory.hpp"
#include "string.h"

#define TCL_CC 1

using namespace std;

Tcl_Interp* gInterp = nullptr;

// public functions //
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
	
	Log_Cout("Tcl variable CurrentLevel linked to C variable\nTcl initialized\n\n");
	
return TCL_OK;
}

void TclCC_Quit() {
	Tcl_UnlinkVar(gInterp, "CurrentLevel");
	Tcl_DeleteInterp(gInterp);
}
