// standard includes
#include "headers/stdinc.h"

// Tcl scripts
#include "headers/tcl.hpp"

// sprite function
#include "headers/sprite_router.hpp"

// level data and functions
#include "headers/levelcode.hpp"

// event triggers
#include "headers/triggers.hpp"

// logging system
#include "headers/log.hpp"

#include "headers/background_images.hpp"
#include "headers/camera.hpp"
#include "headers/levelinfo.hpp"

#include "headers/color.h"

#include <memory>

using namespace std;

#define REPORTERROR(interp)	cerr << "Error at tcl.cc line " << __LINE__ << ": "; \
				reportError(interp);

// private function declarations
static int _TclCC_Level_PlayerLoc(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static int _TclNsCmd_BackgroundFunctions_LoadImage(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static int _TclNsCmd_BackgroundFunctions_SetColor(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static int _TclNsCmd_BackgroundFunction_OffsetImage(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static JColor getColorFromStr(Tcl_Interp* interp, const char* str);
static int reportError(Tcl_Interp*);
static void setErrorResult(Tcl_Interp*, const char*);
static int createNewBackgroundObj(Tcl_Interp*, int, int, float, float);

// interpreter
Tcl_Interp* gInterp = nullptr;

// current level
char* CurrentLevel = nullptr;

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
	Log_Cout("Initializing Tcl\n");

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

	// link CurrentLevel here in C with Tcl's CurrentLevel
	if (Tcl_LinkVar(gInterp, "CurrentLevel", (char*) &CurrentLevel, TCL_LINK_STRING) != TCL_OK) {
	  REPORTERROR(gInterp);
	  return TCL_ERROR;
	}

	// source a script file that creates a namespace called BackgroundFunctions
	Tcl_Eval(gInterp, "source [file join scripts nsbackground.tcl]");

	// name of command
	vector<char> vName(100);

	// link Level_PlayerLoc to _TclCC_Level_PlayerLoc
	strcpy((char*) vName.data(), "Level_PlayerLoc");
	if (! Tcl_CreateObjCommand(gInterp, vName.data(), _TclCC_Level_PlayerLoc, nullptr, nullptr)) {
	  REPORTERROR(gInterp);
	}

	// add the following commands to namespace BackgroundFunctions
	// loadImage: mapped to subcommand "load"
	strcpy((char*) vName.data(), "BackgroundFunctions::loadImage");
	if (! Tcl_CreateObjCommand(gInterp, vName.data(), _TclNsCmd_BackgroundFunctions_LoadImage, program, nullptr)) {
	  REPORTERROR(gInterp);
	}

	// setColor: mapped to subcommand "color"
	strcpy( (char*) vName.data(), "BackgroundFunctions::setColor");
	if (! Tcl_CreateObjCommand(gInterp, vName.data(), _TclNsCmd_BackgroundFunctions_SetColor, program, nullptr)) {
	  REPORTERROR(gInterp);
	}

	// offsetBg
	strcpy( (char*) vName.data(), "BackgroundFunctions::offsetImage" );
	if ( ! Tcl_CreateObjCommand(gInterp, vName.data(), _TclNsCmd_BackgroundFunction_OffsetImage, program, nullptr ) ) {
	  REPORTERROR(gInterp);
	}

	// source the script file that contains Tcl procedures
	Tcl_Eval(gInterp, "source [file join scripts main.tcl]");

return TCL_OK;
}

void TclCC_Quit() {
	Tcl_UnlinkVar(gInterp, "CurrentLevel"); // unlink CurrentLevel
	Tcl_Free(CurrentLevel);                 // free CurrentLevel
	Tcl_DeleteInterp(gInterp);              // delete interpreter
}

// private functions //
void setErrorResult(Tcl_Interp* interp, const char* msg) {
	Tcl_Obj* result = Tcl_NewStringObj(msg, strlen(msg));
	Tcl_SetObjResult(interp, result);
}

int reportError(Tcl_Interp* interp) {
	cerr << Tcl_GetStringResult(interp) << '\n';
	return TCL_ERROR;
}

int loadBackgroundImage(Tcl_Interp* interp, int id, const char* file, uint32_t colorkey) {
	int retval = TCL_OK;

	// file string
	string file2 = "images/backgrounds/";
	file2 += file;

	if (id >= 0 && id < NUM_BGS) {
	  // load image
	  Image* pImage = LevelInfo_BackgroundLayerID(id);
	  pImage->unload();

	  // load file; if colorkey is black, omit colorkey
	  if (colorkey == BLACK)
	  	pImage->open(file2);
	  else
	  	pImage->open(file2, colorkey);

	  // error
	  if (pImage->error()) {
	  	cerr << "Failed to load " << file2 << " -- " << pImage->get_error() << '\n';
	  	retval = TCL_ERROR;
	  }

	  // make width of image 640
	  const SDL_Rect& imgrect = pImage->get_file_rect();
	  pImage->set_blit_size(WIDTH, imgrect.h);
	}

return retval;
}

JColor getColorFromStr(Tcl_Interp* interp, const char* str) {
	stringstream ss;
	JColor retval = {0};

	// argument is not HTML notation
	ss << str;

	if (ss.str().size() != 7) {
	  Tcl_Obj* result = Tcl_NewStringObj("invalid second arg to Level_ConfBackground: must be HTML notation (ie, #ff0000)", -1);
	  Tcl_SetObjResult(interp, result);
	  return retval;
	}

	// add spaces inbetween pairs of characters, like so: ffffff -> ff ff ff
	ss.str("");
	int x = 0;
	while (x < 6) {
	  ss.put(str[x+1]); ++x;
	  ss.put(str[x+1]); ++x;
	  ss.put(' ');
	}

	// convert to integers
	unsigned short int uiRed, uiGreen, uiBlue;
	ss >> hex >> uiRed;
	ss >> uiGreen;
	ss >> uiBlue;

	// fill struct with color information
	retval = ReturnJColor(uiRed, uiGreen, uiBlue, 0xff);

return retval;
}

int createNewBackgroundObj(Tcl_Interp* interp, int id, int type, float factx, float facty) {
	int code = TCL_OK;
	using camera::BGLayers;
	
	if (id < 0 || id >= NUM_BGS) {
	  stringstream ss;
	  ss << "Invalid argument to createNewBackgroundObj, " << id \
	  << ": must be between 0 and " << NUM_BGS-1;
	  setErrorResult(interp, ss.str().c_str());
	  code = TCL_ERROR;
	}
	else {
	  switch (type) {
	  	default:
	  	  {
	  	  	char* sError = TclCC_AllocChar(50);
	  	  	ERRORSTR(sError, createNewBackgroundObj, type);
	  	  	Tcl_SetResult(interp, sError, TCL_DYNAMIC);
	  	  	code = TCL_ERROR;
	  	  	break;
	  	  }
	  	
	  	case BG_X:
	  	  delete BGLayers[id];
	  	  BGLayers[id] = new Background_X(factx, &camera::CamXSpd);
	  	  break;
	  	
	  	case BG_Y:
	  	  delete BGLayers[id];
	  	  BGLayers[id] = new Background_Y(facty, &camera::CamYSpd);
	  	  break;
	  	
	  	case BG_XY:
	  	  delete BGLayers[id];
	  	  BGLayers[id] = new Background_XY(factx, facty, &camera::CamXSpd, &camera::CamYSpd);
	  	  break;
	  } // end switch
	}
	
return code;
}

// args: id x y
int _TclNsCmd_BackgroundFunction_OffsetImage(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	if (objc != 4) {
	  setErrorResult(interp, "wrong # args: should be Level_ConfBackground offset id x y");
	  return TCL_ERROR;
	}
	
	// id number
	int iID;
	if (Tcl_GetIntFromObj(interp, objv[1], &iID) == TCL_ERROR)
	  return TCL_ERROR;
	
	if (iID < 0 || iID >= NUM_BGS) {
	  stringstream ss;
	  ss << "invalid argument to Level_ConfBackground offset, " << iID << ", must be between 0 and " << NUM_BGS-1;
	  setErrorResult(interp, ss.str().c_str());
	  return TCL_ERROR;
	}
	
	// x
	int iX;
	if (Tcl_GetIntFromObj(interp, objv[2], &iX) == TCL_ERROR)
	  return TCL_ERROR;
	
	iX *= TILE_WIDTH;
	
	// y
	int iY;
	if (Tcl_GetIntFromObj(interp, objv[3], &iY) == TCL_ERROR)
	  return TCL_ERROR;
	
	iY *= TILE_HEIGHT;
	
	// create new background object if neccessary
	using camera::BGLayers;
	
	Background_Base* pBg = BGLayers[iID];
	if (pBg == nullptr) {
	  int type = iX != 0 ? BG_X : 0; // if X defined
	  type |= iY != 0 ? BG_Y : 0; // if Y defined
	  createNewBackgroundObj(interp, iID, type, 0, 0);
	  pBg = BGLayers[iID]; // update pointer
	}
	
	// offset the background by the values provided
	switch (pBg->get_type()) {
	  default:
//	  	pBg->x() += iX;
//	  	pBg->y() += iY;
	  	camera::BGMinX = iX;
	  	camera::BGMinY = iY;
	  	break;
	  
	  case BG_X:
//	  	pBg->x() += iX;
	  	camera::BGMinX = iX;
	  	break;
	  
	  case BG_Y:
//	  	pBg->y() += iY;
	  	camera::BGMinY = iY;
	  	break;
	}
	
return TCL_OK;
}

int _TclNsCmd_BackgroundFunctions_SetColor(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	int retval = TCL_OK;
	
	if (objc != 3) {
	  setErrorResult(interp, "wrong # args: should be Level_ConfBackground color colorValue opacity");
	  retval = TCL_ERROR;
	}
	else {
	  JColor colorkey = getColorFromStr( interp, Tcl_GetStringFromObj(objv[1], nullptr) );
	  
	  // opacity
	  int iOpacity = 255;
	  if ( Tcl_GetIntFromObj(interp, objv[2], &iOpacity) == TCL_ERROR )
	  	cerr << "error from background color: " << Tcl_GetStringResult(interp) << '\n';
	  
	  game::bgcolor(colorkey); // change background color
	}
	
	
return retval;
}

// args: idx file ?colorkey?
int _TclNsCmd_BackgroundFunctions_LoadImage(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	int retval = TCL_OK;
	
	if (objc < 3 && objc > 4) {
	  Tcl_Obj* result = Tcl_NewStringObj("wrong # args: should be Level_ConfBackground load idNum file ?colorkey?", -1);
	  Tcl_SetObjResult(interp, result);
	  retval = TCL_ERROR;
	}
	else {
	  string sFile = Tcl_GetStringFromObj(objv[2], nullptr);
	  int iID;
	  if (Tcl_GetIntFromObj(interp, objv[1], &iID) == TCL_ERROR) {
	  	retval = TCL_ERROR;
	  }
	  else {
	  	uint32_t uiColorkey = 0;
	  	if (objc == 4) {
	  	  JColor color = getColorFromStr(interp, Tcl_GetStringFromObj(objv[3], nullptr));
	  	  uiColorkey = JColor2Int(color);
	  	}
	  	
	  	retval = loadBackgroundImage(interp, iID, sFile.c_str(), uiColorkey);
	  }
	}
	
return retval;
}

//int _TclCC_Level_ConfBackground(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
//	static Background_Base* pBg = nullptr;
//	static unique_ptr<int[]> xy(nullptr);
//	
//	short int retval = TCL_OK; // return code
//	int iID; // id of background, or -1 for 
//	double* dpFactors = nullptr;


//	// number of arguments:
//	switch (objc) {
//	  default:
//	  	{
//	  	  stringstream ss;
//	  	  ss << "wrong # args: should be (1) background load idx file colorkey, " \
//	  	  << "(2) background id x_factor y_factor, or (3) background -1 color opacity";

//	  	  // new string
//	  	  char* sResult = Tcl_Alloc(ss.str().length() + 1);
//	  	  strcpy(sResult, ss.str().c_str());

//	  	  Tcl_SetResult(interp, sResult, TCL_DYNAMIC); // set return value
//	  	  retval = TCL_ERROR;
//	  	  break;
//	  	}


//	  

//	  case 5:
//	  	{
//	  	  // first argument must be "load"
//	  	  string sTemp = Tcl_GetStringFromObj(objv[1], nullptr);
//	  	  if (sTemp != "load" && sTemp != "offset") {
//	  	  	retval = TCL_ERROR;
//	  	  	Tcl_SetObjResult(interp, Tcl_NewStringObj("invalid first argument to background (4 args): should be \"load\"", -1));
//	  	  	break;
//	  	  }

//	  	  // id
//	  	  if(Tcl_GetIntFromObj(interp, objv[2], &iID) == TCL_ERROR) {
//	  	  	// error message
//	  	  	Tcl_Obj* result = Tcl_NewStringObj("error: second argument to background is invalid: ", -1);
//	  	  	Tcl_AppendObjToObj(result, Tcl_GetObjResult(interp));
//	  	  	Tcl_SetObjResult(interp, result);
//	  	  	retval = TCL_ERROR; // return code
//	  	  	break;
//	  	  }

//	  	  // background offset 0[id] 3[x] 5[y]
//	  	  if (sTemp == "offset") {
//	  	  	unique_ptr<int[]> args(new int[3]);
//	  	  	
//	  	  	for (int x = 0; x < 3; ++x) {
//	  	  	  // convert objv[2-4] to integers
//	  	  	  if (Tcl_GetIntFromObj(interp, objv[x+2], args.get()+x) == TCL_ERROR) {
//	  	  	  	retval = TCL_ERROR;
//	  	  	  	break;
//	  	  	  }
//	  	  	}
//	  	  	
//	  	  	iID = args[0];
//	  	  	xy = std::move(args); // transfer args to xy
//	  	  	break; // don't complete the rest of the code
//	  	  }
//	  	  if (retval == TCL_ERROR) break;
//	  	  
//	  	  // colorkey
//	  	  uint32_t uiColorkey;
//	  	  {
//	  	  	JColor tempcolor = getColorFromStr(interp, Tcl_GetStringFromObj(objv[4], nullptr));
//	  	  	uiColorkey = JColor2Int(tempcolor);
//	  	  }

//	  	  // file string
//	  	  sTemp = Tcl_GetStringFromObj(objv[3], nullptr);
//	  	  retval = loadBackgroundImage(interp, iID, sTemp.c_str(), uiColorkey);
//	  	  break;
//	  	}

//	  case 4:
//	  	{
//	  	  // id
//	  	  if(Tcl_GetIntFromObj(interp, objv[1], &iID) == TCL_ERROR) {
//	  	  	// error message
//	  	  	Tcl_Obj* result = Tcl_NewStringObj("error: second argument to background is invalid: ", -1);
//	  	  	Tcl_AppendObjToObj(result, Tcl_GetObjResult(interp));
//	  	  	Tcl_SetObjResult(interp, result);
//	  	  	retval = TCL_ERROR; // return code
//	  	  	break;
//	  	  }

//	  	  // change background color
//	  	  if (iID < 0) {
//	  	  	JColor color = getColorFromStr(interp, Tcl_GetStringFromObj(objv[2], nullptr)); // convert string to color value
//	  	  	int op = 255;
//	  	  	Tcl_GetIntFromObj(interp, objv[3], &op); // integeral value
//	  	  	color.alpha = op;
//	  	  	game::bgcolor(color); // change bg color
//	  	  	break;
//	  	  }

//	  	  // argument out of range
//	  	  else if (iID >= NUM_BGS) {
//	  	  	char* sResult = Tcl_Alloc(79);
//	  	  	std::sprintf(sResult, "invalid first arg to Level_ConfBackground: %d. Must between 0 and %d", iID, NUM_BGS); // 68
//	  	  	Tcl_SetResult(interp, sResult, TCL_DYNAMIC);
//	  	  	retval = TCL_ERROR;
//	  	  	break;
//	  	  }

//	  	  // floating point arguments
//	  	  unique_ptr<double[]> temp_dbls(new double[2]);
//	  	  
//	  	  Tcl_GetDoubleFromObj(interp, objv[2], temp_dbls.get());
//	  	  Tcl_GetDoubleFromObj(interp, objv[3], temp_dbls.get()+1);
//	  	  
//	  	  dpFactors = temp_dbls.release();
//	  	  break;
//	  	}
//	}

//	// if success in extracting arguments
//	if (dpFactors) {
//	  int iWhich = 0;
//	  if (dpFactors[0])
//	  	iWhich |= BG_X;
//	  
//	  if (dpFactors[1])
//	  	iWhich |= BG_Y;
//	  
//	  // which class to declare an object of
//	  switch (iWhich) {
//	  	default: break;
//	  	
//	  	case BG_X:
//	  	  pBg = new Background_X(dpFactors[0], &camera::CamXSpd);
//	  	  if (xy) {
//	  	  	pBg->x() = xy[1] * TILE_WIDTH;
//	  	  	xy.reset();
//	  	  }
//	  	  break;
//	  	
//	  	case BG_Y:
//	  	  pBg = new Background_Y(dpFactors[1], &camera::CamXSpd);
//	  	  if (xy) {
//	  	  	pBg->y() = xy[2] * TILE_HEIGHT;
//	  	  	xy.reset();
//	  	  }
//	  	  break;
//	  	
//	  	case BG_XY:
//	  	  pBg = new Background_XY(dpFactors[0], dpFactors[1], &camera::CamXSpd, &camera::CamYSpd);
//	  	  if (xy) {
//	  	  	pBg->x() = xy[1] * TILE_WIDTH;
//	  	  	pBg->y() = xy[2] * TILE_HEIGHT;
//	  	  	xy.reset();
//	  	  }
//	  	  break;
//	  }
//	  
//	  delete[] dpFactors;
//	  dpFactors = nullptr;
//	}

//	// replace old background class with new one
//	if (pBg) {
//	  using camera::BGLayers;
//	  delete BGLayers[iID];
//	  BGLayers[iID] = pBg;
//	  pBg = nullptr;
//	}

//return retval;
//}

int _TclCC_Level_PlayerLoc(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	int iCode;

	// if there are the wrong number of arguments, generate an error
	if (objc != 4) {
	  Tcl_Obj* oResult = Tcl_NewStringObj("wrong # args: should be Level_PlayerLoc exitNum x y", -1);
	  Tcl_SetObjResult(interp, oResult);
	  return TCL_ERROR;
	}

	// first argument is an integer: -1 for main entrance, and above for sub-entrances
	int iEnt;
	iCode = Tcl_GetIntFromObj(interp, objv[1], &iEnt);

	if (iCode != TCL_OK) {
	  iEnt = -1;
	  Log_Cerr("Level_PlayerLoc: unable to parse first argument in player command in %s`s script file\n", CurrentLevel);
	}

	// next two arguments are the X and Y position
	int x, y;
	iCode = Tcl_GetIntFromObj(interp, objv[2], &x);

	if (iCode != TCL_OK) {
	  x = 0;
	  cerr << "Failed to parse second argument for Level_PlayerLoc in " << CurrentLevel << "`s script file\n";
	}

	iCode = Tcl_GetIntFromObj(interp, objv[3], &y);

	if (iCode != TCL_OK) {
	  y = 0;
	  cerr << "Failed to parse third argument for Level_PlayerLoc in " << CurrentLevel << "`s script file\n";
	}

	// if entrance code is -1, that means the we are defining the main player entrace
	if (iEnt == -1) {
	  level::ThePlayer->m_obj.x = x * TILE_WIDTH;
	  level::ThePlayer->m_obj.y = y * TILE_HEIGHT;
	}

	// output result of command
	cout << "Player`s location for entrance " << iEnt << " is at (" << x << ',' << y << ')' << endl;

return TCL_OK;
}
