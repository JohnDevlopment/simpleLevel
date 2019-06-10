# module to set the build options for new_level

# compiler defines
include (ConfigDefines)

# new executable target
add_executable (new_level main.cpp tcl.cpp)

# inclusion directories
target_include_directories (new_level
                            PRIVATE ${TCL_INCLUDE_DIR}
                            PRIVATE ${CUSTOM_INCLUDE_DIR})

# link libraries
target_link_libraries (new_level PUBLIC SDL2 levelLoad tcl8.6 m tk8.6)
install (TARGETS new_level
           RUNTIME
         DESTINATION ${INSTALL_PREFIX}/new_level
         PERMISSIONS
           OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )

install (FILES interface.tk opt.tcl
         DESTINATION ${INSTALL_PREFIX}/new_level
        )

message (STATUS "Files and target will be install in ${INSTALL_PREFIX}/new_level")

message (STATUS "Linked to this project are SDL2, levelLoad, tcl8.6, m, and tk8.6")
message (STATUS "Tcl's headers are at ${TCL_INCLUDE_DIR}")
message (STATUS "Custom headers are at ${CUSTOM_INCLUDE_DIR}")
