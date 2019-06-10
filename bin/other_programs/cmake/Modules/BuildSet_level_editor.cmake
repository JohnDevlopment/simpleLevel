# build rules for level_editor
# set compiler defines and flags
include (ConfigDefines)

# set target information
add_executable (level_editor
                 main.cpp
                 camera.cpp
                 graphics.cpp
                 event.cpp
                 level.cpp
                 mathfunc.cpp
                 sdl.cpp
                 bitfield_class.a
               )

# helps to find SDL2 package
find_package (SDL2)

# set target defines
target_compile_definitions (level_editor PRIVATE WIDTH=640 PRIVATE HEIGHT=480 TILE_WIDTH=16 TILE_HEIGHT=16 _REENTRANT)

# directories for library headers
target_include_directories (level_editor
                             PRIVATE ${TCL_INCLUDE_DIR}
                             PRIVATE ${SDL_INCLUDE_DIR}
                             PRIVATE ${CUSTOM_INCLUDE_DIR}
                           )

# additional options to the linker to support SDL2
target_link_options (level_editor PRIVATE "LINKER:-rpath,/usr/local/lib,--enable-new-dtags" "SHELL:-L${SDL2_LIBDIR}")

# include libraries
target_link_libraries (level_editor
                        PUBLIC
                         levelLoad
                         SDL2
                         SDL2_image
                         tcl8.6
                         tk8.6
                         m
                         image_class
                      )

# install rules for target and its files
install (TARGETS level_editor RUNTIME DESTINATION ${INSTALL_PREFIX}
         PERMISSIONS
          OWNER_READ OWNER_WRITE OWNER_EXECUTE
          GROUP_READ GROUP_EXECUTE
          WORLD_READ WORLD_EXECUTE)

install (FILES 
          fileExists lved.sh lvledopen.tk
         DESTINATION ${INSTALL_PREFIX}
         PERMISSIONS
          OWNER_READ OWNER_WRITE OWNER_EXECUTE
          GROUP_READ GROUP_EXECUTE
          WORLD_READ WORLD_EXECUTE)

install (FILES
          images.zip README.txt
         DESTINATION ${INSTALL_PREFIX}
         PERMISSIONS
          OWNER_READ OWNER_WRITE
          GROUP_READ GROUP_WRITE
          WORLD_READ)
