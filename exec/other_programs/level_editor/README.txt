=== Requirements ===
SDL2 development and runtime libraries (libSDL2 & libSDL2_image)
Tcl & Tk development libraries (libtcl8.6 & libtk8.6 & libm)
My custom levelLoad library (liblevelLoad)
My custom image_class lbirary (libimage_class)

=== Compilation Instructions ===
Step 1. Go to the "build" directory and type this into the commandline:
$ cmake ..

Step 2. Type 'make' into the commandline to build the project.

Step 3. Type "make install" into the commandline to install the project into its default install directory, which is the "bin" subdirectory under the same directory as the CMakeLists.txt and all the source files.

Step 4. Step into the directory where the installed files are an unzip images.zip and then remove it.

--- Extra Options ---
There are ways to customize the build of level_editor, and for that matter, header_editor and new_level. CMake has what are called "cache variables". Four are defined for the three projects listed:
	TCL_INCLUDE_DIR = A path to the directory where Tcl header files reside
	SDL_INCLUDE_DIR = A path to the directory where SDL 2.0+ header files reside
	CUSTOM_INCLUDE_DIR = A path to the directory where my custom header files reside
	INSTALL_PREFIX = Where to install the project after compilation

To configure these paths, after doing the first cmake command, go into the newly generated CMakeCache.txt and look for the above-listed names. Set their respective values to what you want.

=== How to Use this Program ===
