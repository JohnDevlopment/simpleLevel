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

#define REPORTERROR(interp)	cerr << "Error at tcl.cc line " << __LINE__ << ": "; \
				reportError(interp);

#ifndef JOIN
 #define JOIN(a,b)  JOIN1(a,b)
 #define JOIN1(a,b) a##b
#endif

// functions from other source files
void _gm_level_saveMusData(MusData* mus);
MusData* _gm_level_getMusData();

// private function declarations
static int _TclCC_Level_PlayerLoc(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static int _TclCC_SendData(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static int _TclNsCmd_BackgroundFunctions_LoadImage(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static int _TclNsCmd_BackgroundFunctions_SetColor(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static int _TclNsCmd_BackgroundFunction_OffsetImage(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static int _TclNsCmd_BackgroundFunction_SetSpeeds(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static int _TclNsCmd_Sound_Music(ClientData, Tcl_Interp*, int, Tcl_Obj* const[]);
static JColor getColorFromStr(Tcl_Interp* interp, const char* str);
static int reportError(Tcl_Interp*);
static void setErrorResult(Tcl_Interp*, const char*);
static int createNewBackgroundObj(Tcl_Interp*, int, int, float, float);

Tcl_Interp* gInterp = nullptr;
char* CurrentLevel = nullptr;

#include "tcl/tcl_pointer_obj_type.cxx"

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
	Tcl_ObjType* typePtr;
	
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

	// source scripts that define command namespaces
	Tcl_Eval(gInterp, "source [file join scripts nsbackground.tcl]");
	Tcl_Eval(gInterp, "source [file join scripts nssound.tcl]");

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

	// setSpeeds
	strcpy( (char*) vName.data(), "BackgroundFunctions::setSpeeds" );
	if ( ! Tcl_CreateObjCommand(gInterp, vName.data(), _TclNsCmd_BackgroundFunction_SetSpeeds, program, nullptr) ) {
	  REPORTERROR(gInterp);
	}

	// add the following commands to namespace Sound
	// music: mapped to subcommand "music"
	typePtr = _tcl_createNewObjType_PtrObj();
	strcpy( (char*) vName.data(), "Sound::musicCmd" );
	if ( ! Tcl_CreateObjCommand(gInterp, vName.data(), _TclNsCmd_Sound_Music, typePtr, _tcl_deleteObjType) ) {
	  REPORTERROR(gInterp);
	}

	// add command: SendData
	strcpy( (char*) vName.data(), "SendData" );
	if ( ! Tcl_CreateObjCommand(gInterp, vName.data(), _TclCC_SendData, nullptr, nullptr) ) {
	  REPORTERROR(gInterp);
	}

	// source the script file that contains Tcl procedures
	Tcl_Eval(gInterp, "source [file join scripts main.tcl]");

return TCL_OK;
}

void TclCC_Quit() {
	Tcl_UnlinkVar(gInterp, "CurrentLevel");
	Tcl_Free(CurrentLevel);
	Tcl_DeleteInterp(gInterp);
}

//////////////// private functions /////////////////////
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

int _TclNsCmd_BackgroundFunction_SetSpeeds(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	if (objc != 4) {
	  setErrorResult(interp, "wrong # args: should be Level_ConfBackground speeds id x_per y_per");
	  return TCL_ERROR;
	}
	
	int iID;
	float fX, fY;
	
	// id number
	if ( Tcl_GetIntFromObj(interp, objv[1], &iID) == TCL_ERROR ) return TCL_ERROR;
	// out of range
	if (iID < 0 || iID >= NUM_BGS) {
	  stringstream ss;
	  ss << "invlaid argument to Level_ConfBackground speeds, " << iID << ", must be between 0 and " << NUM_BGS-1;
	  setErrorResult(interp, ss.str().c_str());
	  return TCL_ERROR;
	}
	
	// x fraction
	double temp_dArg;
	
	if (Tcl_GetDoubleFromObj(interp, objv[2], &temp_dArg) == TCL_ERROR)
	  return TCL_ERROR;
	
	fX = temp_dArg;
	
	// y fraction
	if (Tcl_GetDoubleFromObj(interp, objv[3], &temp_dArg) == TCL_ERROR)
	  return TCL_ERROR;
	
	fY = temp_dArg;
	
	
	using camera::BGLayers;
	Background_Base* ptr = BGLayers[iID];
	
	if (ptr)
	  delete ptr;
	
	// create a new background vector with the given speed factors
	int type = (fX ? BG_X : 0) | (fY ? BG_Y : 0);
	createNewBackgroundObj(interp, iID, type, fX, fY);
	
return TCL_OK;
}

int _TclNsCmd_BackgroundFunction_OffsetImage(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	if (objc != 4) {
	  setErrorResult(interp, "wrong # args: should be Level_ConfBackground offset id x y");
	  return TCL_ERROR;
	}
	
	int iID, iX, iY;
	
	// id number
	if (Tcl_GetIntFromObj(interp, objv[1], &iID) == TCL_ERROR)
	  return TCL_ERROR;
	
	if (iID < 0 || iID >= NUM_BGS) {
	  stringstream ss;
	  ss << "invalid argument to Level_ConfBackground offset, " << iID << ", must be between 0 and " << NUM_BGS-1;
	  setErrorResult(interp, ss.str().c_str());
	  return TCL_ERROR;
	}
	
	// x
	if (Tcl_GetIntFromObj(interp, objv[2], &iX) == TCL_ERROR)
	  return TCL_ERROR;
	
	iX *= TILE_WIDTH;
	
	// y
	if (Tcl_GetIntFromObj(interp, objv[3], &iY) == TCL_ERROR)
	  return TCL_ERROR;
	
	iY *= TILE_HEIGHT;
	
	// create new background object if neccessary
	using camera::BGLayers;
	
	Background_Base* pBg = BGLayers[iID];
	if (pBg == nullptr) {
	  int type = ((iX != 0) ? BG_X : 0) | ((iY != 0) ? BG_Y : 0);
	  createNewBackgroundObj(interp, iID, type, 0, 0);
	  pBg = BGLayers[iID]; // update pointer
	}
	
	// offset the background by the values provided
	camera::BGMinX = iX;
	camera::BGMinY = iY;
	
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
	  using level::ThePlayer;
	  ThePlayer->m_obj.set_xy(x * TILE_WIDTH, y * TILE_HEIGHT);
	}

	// output result of command
	cout << "Player`s location for entrance " << iEnt << " is at (" << x << ',' << y << ')' << endl;

return TCL_OK;
}

// args: SendData <string> <PtrObj>
int _TclCC_SendData(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	if (objc != 3) {
	  setErrorResult(interp, "wrong # args: should be SendData <string> <PtrObj>");
	  return TCL_ERROR;
	}
	
	Tcl_Obj* ptrObj = objv[2];
	std::string sArg = Tcl_GetStringFromObj(objv[1], nullptr);
	
	assert(ptrObj != nullptr);
	assert(ptrObj->typePtr != nullptr);
	assert( strcmp(ptrObj->typePtr->name, "Pointer") == 0 );
	
	if (sArg == "gm_level_music") {
	  PointerObjInfo* info = (PointerObjInfo*) ptrObj->internalRep.otherValuePtr;
	  assert(info->length == sizeof(MusData));
	  
	  _gm_level_saveMusData( reinterpret_cast<MusData*>(info->address) );
	  info->address = nullptr;
	  info->length = 0;
	}
	
return TCL_OK;
}

//////////// functions to help _TclNsCmd_Sound_Music /////////////////
// syntax: Level_SoundCmd music load <file> ?<vol = MAX_VOLUME>? ?<loop = 0.0>?
// args start at "load" | 1. load 2. <file> 3. ?<vol = MAX_VOLUME>? 4. ?<loop = 0.0>?
static int loadMusicFileIntoObj(Tcl_ObjType* typePtr, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	Tcl_Obj* result;
	PointerObjInfo* ptrInfo;
	char sFile[100];
	MusData* mus;
	int iVolume = MAX_VOLUME;
	
	if (objc < 2 && objc > 4) {
	  setErrorResult(interp, "wrong # args: should be Level_SoundCmd music load <file> ?<vol = MAX_VOLUME>? ?<loop = 0.0>?");
	  return TCL_ERROR;
	}
	
	// allocate TclObj structure
	result = Tcl_NewObj();
	if (result == nullptr) {
	  return TCL_ERROR;
	}
	
	// load music file
	std::memset(sFile, 0, 100);
	{
	  const char* temp = Tcl_GetStringFromObj(objv[2], nullptr);
	  
	  strcpy(sFile, "audio/music/");
	  String_strlcat(sFile, temp, 100);
	}
	mus = Sound_LoadMUS(sFile);
	if (! mus) {
	  setErrorResult(interp, "Failed to load music file");
	  Tcl_DecrRefCount(result);
	  return TCL_ERROR;
	}
	
	// get arguments and pass to MusData
	if (objc >= 4) {
	  Tcl_GetIntFromObj(nullptr, objv[3], &iVolume);
	  if (iVolume < 0)
	  	iVolume = 0;
	  else if (iVolume > MAX_VOLUME)
	  	iVolume = MAX_VOLUME;
	  
	  MusData_SetVolume(mus, iVolume);
	}
	
	{
	  double temp = 0.0;
	  
	  if (objc == 5) {
	  	Tcl_GetDoubleFromObj(nullptr, objv[4], &temp);
	  	if (temp < 0.0)
	  	  temp = 0.0;
	  	else if (temp > 60.0)
	  	  temp = 60.0;
	  }
	  mus->lpos = temp;
	}
	
	// allocate PointerObjInfo to contain MusData and be managed internally by Tcl
	ptrInfo = (PointerObjInfo*) TclCC_AllocChar(sizeof(PointerObjInfo));
	ptrInfo->type = PTROBJ_MUSDATA;
	ptrInfo->address = mus;
	ptrInfo->length = sizeof(MusData);
	
	// assign struct to object
	result->internalRep.otherValuePtr = ptrInfo;
	result->typePtr = typePtr;
	result->bytes = nullptr;
	
	// set object as result of function
	Tcl_SetObjResult(interp, result);
	
return TCL_OK;
}

// syntax: Level_SoundCmd music play <loop_pos> ?<loops>? ?<volume>? ?<dphase>?
// args: start at "play" | 1. play 2. loop_pos 3. loops 4. volume 5. dphase
static int playLevelMusic(Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	int retval;
	int iVolume = -1;
	int iLoops = 0;
	int iPhase = -1;
	double dLoopPos = -1.0;
	std::string sErrors;
	MusData* mus;
	
	if (objc < 3 || objc > 6) {
	  setErrorResult(interp, "wrong # args: should be Level_SoundCmd music play <loop_pos> ?<loops>? ?<volume>? ?<dphase>?");
	  return TCL_ERROR;
	}
	
	// checks any errors from a Tcl lib function
	auto AppendError = [&sErrors](const char* _error, int _code) -> void {
	  if (_code == TCL_ERROR) {
	  	if (_error && strlen(_error) > 0) {
	  	  sErrors += _error;
	  	  sErrors += "\n";
	  	}
	  }
	};
	
	// retrieve arguments
	sErrors = "\t";
	for (int x = 0; x < objc; ++x) {
	  switch (x) {
	  	default: break;
	  	
	  	case 2:
	  	  retval = Tcl_GetDoubleFromObj(interp, objv[x], &dLoopPos);
	  	  AppendError( Tcl_GetStringResult(interp), retval );
	  	  break;
	  	
	  	case 3:
	  	  retval = Tcl_GetIntFromObj(interp, objv[x], &iLoops);
	  	  AppendError( Tcl_GetStringResult(interp), retval );
	  	  break;
	  	
	  	case 4:
	  	  retval = Tcl_GetIntFromObj(interp, objv[x], &iVolume);
	  	  AppendError( Tcl_GetStringResult(interp), retval );
	  	  break;
	  	
	  	case 5:
	  	  retval = Tcl_GetIntFromObj(interp, objv[x], &iPhase);
	  	  AppendError( Tcl_GetStringResult(interp), retval );
	  	  break;
	  }
	}
	
	// iLoops -1, 0+
	if (iLoops < -1) {
	  iLoops = -1;
	}
	
	// dLoopPos 0 ~ 60
	if (dLoopPos < 0.0) {
	  dLoopPos = 0.0;
	}
	else if (dLoopPos > 60.0) {
	  dLoopPos = 60.0;
	}
	
	// log errors that occurred
	if (retval == TCL_ERROR) {
	  Log_Cerr("Errors present in tcl.cc, playLevelMusic:\n%s", sErrors.c_str());
	  retval = TCL_OK;
	}
	
	// attempt to play the music in question
	mus = _gm_level_getMusData();
	if (mus) {
	  mus->lpos = dLoopPos;
	  if ( Sound_PlayMusic(mus, iLoops) < 0 ) {
	  	retval = TCL_ERROR;
	  	setErrorResult(interp, "failed to play music for level");
	  }
	  else if (iLoops > 0) {
	  	Sound_PauseMusic();
	  	if (iVolume >= 0 && iVolume <= MAX_VOLUME) {
	  	  Sound_VolumeMusic(iVolume);
	  	}
	  }
	}
	
return retval;
}

// syntax: Level_SoundCmd music fade <ms>
// args: start at "fade" | 1. fade 2. ms
static int fadeOutMusic(Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	int iMs;
	
	if (objc != 3) {
	  setErrorResult(interp, "wrong # args: should be Level_SoundCmd music fade <ms>");
	  return TCL_ERROR;
	}
	
	if ( Tcl_GetIntFromObj(interp, objv[2], &iMs) == TCL_ERROR ) {
	  return TCL_ERROR;
	}
	
	MusData* mus = _gm_level_getMusData();
	if (mus) {
	  Sound_FadeOutMusic(iMs);
	}
	
return TCL_OK;
}

// args: Level_SoundCmd music <load|play> ?args...?
int _TclNsCmd_Sound_Music(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
	int retval;
	
	if (objc < 2) {
	  setErrorResult(interp, "Not enough args to Level_SoundCmd: subcommand should be next");
	  return TCL_ERROR;
	}
	
	std::string sArg = Tcl_GetStringFromObj(objv[1], nullptr);
	if (sArg == "load") {
	  retval = loadMusicFileIntoObj((Tcl_ObjType*) cd, interp, objc, objv);
	}
	else if (sArg == "play") {
	  retval = playLevelMusic(interp, objc, objv);
	}
	else if (sArg == "stop") {
	  Sound_HaltMusic();
	  retval = TCL_OK;
	}
	else if (sArg == "fade") {
	  retval = fadeOutMusic(interp, objc, objv);
	}
	else {
	  std::string sError = "invalid subcommand \"";
	  sError += sArg + "\"";
	  sError += ": should be one of load, play";
	  setErrorResult(interp, sError.c_str());
	  return TCL_ERROR;
	}
	
return retval;
}
