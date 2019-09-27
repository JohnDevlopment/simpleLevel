# location of headers
set (HEADERDIR ${PROJECT_SOURCE_DIR}/src/headers)

# location of library sources
set (ALIBDIR ${PROJECT_SOURCE_DIR}/src/libs)


# generate static libraries #
add_library (timer_class STATIC ${ALIBDIR}/timer_class.cpp)
add_library (bitfield_class STATIC ${ALIBDIR}/bitfield_class.cc)

# header directories for timer_class
target_include_directories (timer_class
                             PRIVATE ${SDL_INCLUDE_DIR}
                           )
target_compile_options (timer_class
                         PRIVATE "SHELL:-iquote ${HEADERDIR}"
                         PRIVATE "SHELL:$<$<BOOL:${DEBUG}>:-g>"
                         PRIVATE "SHELL:$<$<NOT:$<BOOL:${DEBUG}>>:-O3>"
                       )
target_compile_definitions (timer_class
                             PRIVATE _REENTRANT
                           )

# header directories for bitfield_class
target_compile_options (bitfield_class
                         PRIVATE "SHELL:-iquote ${HEADERDIR}"
                         PRIVATE "SHELL:$<$<BOOL:${DEBUG}>:-g>"
                         PRIVATE "SHELL:$<$<NOT:$<BOOL:${DEBUG}>>:-O3>"
                       )


# generate main executable #
add_executable (simpleLevel ${SOURCE_FILES})

# compiler options
target_include_directories (simpleLevel
                             PRIVATE ${SDL_INCLUDE_DIR}
                             PRIVATE ${TCL_INCLUDE_DIR}
                           )
target_compile_options (simpleLevel
                         PRIVATE "SHELL:-iquote ${HEADERDIR}"
                         PRIVATE "SHELL:-iquote ${HEADERDIR}/fileio"
                         PRIVATE "SHELL:-iquote ${HEADERDIR}/sound"
                         PRIVATE "SHELL:-iquote ${HEADERDIR}/components"
                         PRIVATE "SHELL:$<$<BOOL:${DEBUG}>:-g>"
                         PRIVATE "SHELL:$<$<NOT:$<BOOL:${DEBUG}>>:-O3>"
                       )
target_compile_definitions (simpleLevel
                             PRIVATE
                              TILE_WIDTH=32
                              TILE_HEIGHT=32
                              FPS=30
                              WIDTH=640
                              HEIGHT=480
                              PI=3.14159265
                              OGG_HEADER=<vorbis/vorbisfile.h>
                              OGG_FILE_LIB="/usr/local/lib/libvorbisfile.so"
                              NUMBER_OF_CODED_TILES=56
                              _REENTRANT
                           )

# link exectuable with libraries and set linker options
target_link_options (simpleLevel PRIVATE "LINKER:-rpath,/usr/local/lib,--enable-new-dtags" "SHELL:-L/usr/local/lib")

target_link_libraries (simpleLevel
                        PRIVATE
                         bitfield_class dl image_class levelLoad m SDL2 SDL2_image SDL2_ttf tcl8.6 timer_class tk8.6
                      )
