=== Compilation Instructions ===
Step 1. Go to the "build" directory and type this into the commandline:
$ cmake ..

Step 2. Type 'make' into the commandline to build the project.

Step 3. Type "make install" into the commandline to install the project into its default install directory, which is the "bin" subdirectory under the same directory as the CMakeLists.txt and all the source files.

--- Extra Options ---
There are ways to customize the build of header_editor. CMake has what are called "cache variables". Four are defined for the project listed:
	TCL_INCLUDE_DIR = A path to the directory where Tcl header files reside
	SDL_INCLUDE_DIR = A path to the directory where SDL 2.0+ header files reside
	CUSTOM_INCLUDE_DIR = A path to the directory where my custom header files reside
	INSTALL_PREFIX = Where to install the project after compilation

=== How to Use this Program ===
Once it's compiled, just execute header_editor. Click open and locate a .dat file that was created by new_level. The text box gives some basic information about the level, such as its size and how many tiles in it. It also lists in separate boxes the background number, tileset number, and music number. You can change these values and then save them to the level by clicking the save button.
