# location of headers
set (HEADERDIR ${PROJECT_SOURCE_DIR}/src/headers)

# location of library sources
set (ALIBDIR ${PROJECT_SOURCE_DIR}/src/libs)

# locate SDL2 in a package
find_package (SDL2 REQUIRED)


# compile object file from res.cxx and add it to the project #
add_library (os_resobj OBJECT ${PROJECT_ROOT_DIR}/src/res/${PLATFORM_DIR}/res.cxx)
target_include_directories (os_resobj
                             PRIVATE ${HEADERDIR}
                           )
target_compile_options (os_resobj
                         PRIVATE "SHELL:-iquote ${HEADERDIR}"
                       )
set_os_compiler_defines (os_resobj)


# generate static libraries #
add_library (timer_class STATIC ${ALIBDIR}/timer_class.cpp)
add_library (bitfield_class STATIC ${ALIBDIR}/bitfield_class.cc)

# header directories for timer_class
target_include_directories (timer_class
                             PRIVATE ${SDL2_INCLUDE_DIR}
                           )
target_compile_options (timer_class
                         PRIVATE "SHELL:-iquote ${HEADERDIR}"
                         PRIVATE "SHELL:$<$<BOOL:${DEBUG}>:-g>"
                         PRIVATE "SHELL:$<$<NOT:$<BOOL:${DEBUG}>>:-O3>"
                       )
target_compile_definitions (timer_class
                             PRIVATE
                              _REENTRANT
                           )

# header directories for bitfield_class
target_compile_options (bitfield_class
                         PRIVATE "SHELL:-iquote ${HEADERDIR}"
                         PRIVATE "SHELL:$<$<BOOL:${DEBUG}>:-g>"
                         PRIVATE "SHELL:$<$<NOT:$<BOOL:${DEBUG}>>:-O3>"
                       )


# generate main executable #
add_executable (simpleLevel ${SOURCE_FILES})
unset (SOURCE_FILES)
set_os_compiler_defines (simpleLevel)

# compiler options
target_include_directories (simpleLevel
                             PRIVATE ${SDL2_INCLUDE_DIR}
                             PRIVATE ${TCL_INCLUDE_DIR}
                           )
target_compile_options (simpleLevel
                         PRIVATE "SHELL:-iquote ${HEADERDIR}"
                         PRIVATE "SHELL:-iquote ${HEADERDIR}/components"
                         PRIVATE "SHELL:-iquote ${HEADERDIR}/fileio"
                         PRIVATE "SHELL:-iquote ${HEADERDIR}/res"
                         PRIVATE "SHELL:-iquote ${HEADERDIR}/sound"
                         PRIVATE "SHELL:-iquote ${HEADERDIR}/video"
                         PRIVATE "SHELL:$<$<BOOL:${DEBUG}>:-g>"
                         PRIVATE "SHELL:$<$<NOT:$<BOOL:${DEBUG}>>:-O3>"
                       )
target_compile_definitions (simpleLevel
                             PRIVATE
                              FPS=30
                              HEIGHT=480
                              NUMBER_OF_CODED_TILES=56
                              OGG_HEADER=<vorbis/vorbisfile.h>
                              OGG_FILE_LIB="/usr/local/lib/libvorbisfile.so"
                              PI=3.14159265
                              _REENTRANT
                              TILE_HEIGHT=32
                              TILE_WIDTH=32
                              WIDTH=640
                              $<$<NOT:$<BOOL:${DEBUG}>>:NDEBUG>
                           )

# link exectuable with libraries and set linker options
target_link_options (simpleLevel PRIVATE "LINKER:-rpath,${SDL2_LIBDIR},--enable-new-dtags" "SHELL:-L${SDL2_LIBDIR}")

target_link_libraries (simpleLevel
                        PRIVATE
                         bitfield_class dl levelLoad m os_resobj ${SDL2_LIBRARY} SDL2_image
                         SDL2_ttf tcl8.6 timer_class tk8.6
                      )
