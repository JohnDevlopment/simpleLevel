#include "tcl.h"

using std::cerr;

static int SetTclCommands(Tcl_Interp* interp);
static int ReadHeader(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);
static int WriteHeader(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

void AllocChar(char** ptr, size_t n) {
    // pointer argument is NULL
    if (ptr == nullptr) {
        bad_option e("AllocChar: ptr is NULL");
        throw e;
    }

    *ptr = Tcl_Alloc(n);

    // failed to allocate memory
    if (*ptr == nullptr) {
        std::bad_alloc e;
        throw e;
    }
}

int InitTcl(Tcl_Interp** interp) {
    // create interpreter
    *interp = Tcl_CreateInterp();

    if (*interp == nullptr) {
        cerr << "Tcl: no interpreter!\n";
        return TCL_ERROR;
    }

    // start Tcl
    if ( Tcl_Init(*interp) ) {
        cerr << "Tcl: Tcl failed to load\n";
        return TCL_ERROR;
    }

    // start Tk
    if ( Tk_Init(*interp) ) {
        cerr << "Tcl: Tk failed to load\n";
        return TCL_ERROR;
    }

    // register commands
    SetTclCommands(*interp);

    // evaluate script file
    Tcl_EvalFile(*interp, "interface.tk");

return TCL_OK;
}

int ReadHeader(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
    LevelHeader header;
    std::vector<Tile> vtiles;
    char* error = nullptr;

    // error string
    Calloc(error, 101);

    // to few arguments
    if (objc != 2) {
        strcpy(error, "wrong # args: must be ReadHeader filename");
        Tcl_SetResult(interp, error, TCL_DYNAMIC);
        return TCL_ERROR;
    }

    // file name
    char* sFile = Tcl_GetStringFromObj(objv[1], nullptr);

    // read header into struct
    if ( Level_ReadFile(sFile, header, vtiles) <= 0 ) {
        strncpy(error, Level_GetError(), 100);
        Tcl_SetResult(interp, error, TCL_DYNAMIC);
        return TCL_ERROR;
    }

    // free error string
    FreeChar(error);

    // clear pointer
    sFile = nullptr;

    // construct list of values
    Tcl_Obj* objlist[6];

    objlist[0] = Tcl_NewIntObj(header.width);
    objlist[1] = Tcl_NewIntObj(header.height);
    objlist[2] = Tcl_NewIntObj(header.background);
    objlist[3] = Tcl_NewIntObj(header.tileset);
    objlist[4] = Tcl_NewIntObj(header.music);
    objlist[5] = Tcl_NewIntObj(vtiles.size());

    Tcl_Obj* listobj = Tcl_NewListObj(6, objlist);

    // set list as return value
    Tcl_SetObjResult(interp, listobj);

return TCL_OK;
}

int WriteHeader(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
    char* error = nullptr;
    Calloc(error, 101);

    // wrong # args
    if (objc != 5) {
        std::strcpy(error, "wrong # args: should be WriteHeader file bg tileset music");
        Tcl_SetResult(interp, error, TCL_DYNAMIC);
        return TCL_ERROR;
    }

    // get string argument
    char* sFile = Tcl_GetStringFromObj(objv[1], nullptr);
    if (sFile == nullptr) return TCL_ERROR;

    // read file header and tile array
    LevelHeader header;
    std::vector<Tile> vtiles;

    if ( Level_ReadFile(sFile, header, vtiles) <= 0 ) {
        strncpy(error, Level_GetError(), 100);
        Tcl_SetResult(interp, error, TCL_DYNAMIC);
        return TCL_ERROR;
    }

    // get arguments 3 - 5
    {
        int buffer, code;
        uint8_t* ptrs[] = {
            &header.background,
            &header.tileset,
            &header.music
        };

        for (int x = 0; x < 3; ++x) {
            code = Tcl_GetIntFromObj(interp, objv[x+2], &buffer);
            if (code != TCL_OK) return TCL_ERROR;

            *ptrs[x] = static_cast<uint8_t>(buffer);
        }
    }

    // write header to file
    if ( Level_WriteFile(sFile, header, vtiles) <= 0 ) {
        std::strncpy(error, Level_GetError(), 100);
        Tcl_SetResult(interp, error, TCL_DYNAMIC);
        return TCL_ERROR;
    }

    // free memory
    FreeChar(error);

    // set return value
    Tcl_SetObjResult(interp, Tcl_NewIntObj(0));

return TCL_OK;
}

int SetTclCommands(Tcl_Interp* interp) {
    char name[31];
    std::memset(name, 0, 31);

    // set Tcl command that reads a header from file
    std::strcpy(name, "ReadHeader");
    Tcl_CreateObjCommand(interp, name, ReadHeader, nullptr, nullptr);

    // set Tcl command that writes the header to file
    std::strcpy(name, "WriteHeader");
    Tcl_CreateObjCommand(interp, name, WriteHeader, nullptr, nullptr);

return TCL_OK;
}
