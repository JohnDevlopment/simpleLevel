#include "tcl.h"

using std::cerr;
using std::string;

static int SetTclCommands(Tcl_Interp* interp);

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

int WriteLevel(ClientData cd, Tcl_Interp* interp, int objc,
               Tcl_Obj* const objv[])
{
    // file name
    const char* file = nullptr;

    // wrong # args
    if (objc != 7) {
        Tcl_Obj* result = Tcl_NewStringObj("wrong # args: should be WriteLevel filename width height background tileset music", -1);
        Tcl_SetObjResult(interp, result);
        return TCL_ERROR;
    }

    // filename
    file = Tcl_GetStringFromObj(objv[1], nullptr);

    // level header
    generic_class<LevelHeader> header;

    // vector array of tiles
    std::vector<Tile> vtiles;

    {
        int buffer = 0;

        // integer arguments put into header
        Tcl_GetIntFromObj(interp, objv[2], &buffer);
        header->width = buffer;

        Tcl_GetIntFromObj(interp, objv[3], &buffer);
        header->height = buffer;

        Tcl_GetIntFromObj(interp, objv[4], &buffer);
        header->background = buffer;

        Tcl_GetIntFromObj(interp, objv[5], &buffer);
        header->tileset = buffer;

        Tcl_GetIntFromObj(interp, objv[6], &buffer);
        header->music = buffer;

        // save byte order to header
        buffer = 1;
        if (is_bigendian(buffer))
            header->endian = CUSTOM_BITS_BIG_ENDIAN;
        else
            header->endian = CUSTOM_BITS_LITTLE_ENDIAN;
    }

    // resize vector array
    {
        const size_t ntiles = header->width * header->height;
        if (! ntiles) {
            char result[] = "error from WriteHeader: either width or height was 0";
            Tcl_SetResult(interp, result, TCL_VOLATILE);
            return TCL_ERROR;
        }

        vtiles.assign(ntiles, Tile());
    }

    #ifdef DEBUG
    std::cout << "level width = " << (int) header->width << ", height = " \
    << (int) header->height << ", background = " << (int) header->background;

    std::cout << ", tileset = " << (int) header->tileset << ", music = " \
    << (int) header->music << std::endl;

    std::cout << "Write all this to: " << file << std::endl;
    #endif // DEBUG

    {
        const int byteswritten = Level_WriteFile(file, header.get(), vtiles);

        if (byteswritten < 0) {
            char* result = nullptr;
            Calloc(result, 151);
            std::sprintf(result, "error from WriteFile: %s", Level_GetError());
            Tcl_SetResult(interp, result, TCL_DYNAMIC);
            return TCL_ERROR;
        }

        #ifdef DEBUG
        int bytesneeded = sizeof(LevelHeader) + sizeof(Tile) * (header->width * header->height);
        std::cout << byteswritten << " out of " << bytesneeded << " bytes written" << std::endl;
        #endif // DEBUG
    }

return TCL_OK;
}

int SetTclCommands(Tcl_Interp* interp) {
    char name[31];
    std::memset(name, 0, 31);

    std::strcpy(name, "WriteLevel");
    Tcl_CreateObjCommand(interp, name, WriteLevel, nullptr, nullptr);

return TCL_OK;
}
