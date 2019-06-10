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
target_link_libraries (new_level PUBLIC levelLoad tcl8.6 m tk8.6)

# where to install the files
install (TARGETS new_level
           RUNTIME
         DESTINATION ${INSTALL_PREFIX}
         PERMISSIONS
           OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )

install (FILES interface.tk opt.tcl README.txt
         DESTINATION ${INSTALL_PREFIX}
        )
