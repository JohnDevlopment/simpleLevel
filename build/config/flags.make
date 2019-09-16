# compiler flags
# c++ standard
CXX_STANDARD = gnu++14

# inclusion directories
CXX_INCLUDES := -I/usr/include/tcl8.6 -I/usr/local/include/custom -iquote $(MAKE_ROOT_DIR)/src/headers -iquote $(MAKE_ROOT_DIR)/src/headers/sprites -iquote $(MAKE_ROOT_DIR)/src/headers/components -iquote $(MAKE_ROOT_DIR)/src/headers/fileio

# defines
CXX_FLAGS = --std=$(CXX_STANDARD) $(foreach def, $(shell cat build/config/defines.txt), -D$(def)) $(CXX_INCLUDES) $(shell sdl2-config --cflags)
