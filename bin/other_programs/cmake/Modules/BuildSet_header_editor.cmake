# module to set the build options for header_editor

# compiler defines
include (ConfigDefines)

# new executable target
add_executable (header_editor main.cpp tcl.cpp)

# inclusion directories
target_include_directories (header_editor
                            PRIVATE ${TCL_INCLUDE_DIR}
                            PRIVATE ${CUSTOM_INCLUDE_DIR})

# link libraries
target_link_libraries (header_editor PUBLIC levelLoad tcl8.6 m tk8.6)
install (TARGETS header_editor
           RUNTIME
         DESTINATION ${INSTALL_PREFIX}
         PERMISSIONS
           OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )

install (FILES interface.tk README.txt
         DESTINATION ${INSTALL_PREFIX}
        )
