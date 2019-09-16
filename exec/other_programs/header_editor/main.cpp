#include <iostream>
#include <cstdlib>
#include "tcl.h"

using namespace std;

static void quit();

Tcl_Interp* interp = nullptr;

int main() {
    atexit(quit);

    // start up Tcl
    InitTcl(&interp);

    // Tk event loop
    Tk_MainLoop();

return 0;
}

void quit() {
    Tcl_DeleteInterp(interp);
}
