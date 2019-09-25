OUTPUT_OPTION = -o $@

# recipe to compile object file from C++ source
COMPILE.o = $(CXX) -c $(CXX_FLAGS) $(OUTPUT_OPTION) $<

# compiler
CXX = g++

# c++ standard
CXX_STANDARD = gnu++14

# inclusion directories
CXX_INCLUDES = -I/usr/include/tcl8.6 -I/usr/local/include/custom -iquote $(MAKE_ROOT_DIR)/src/headers -iquote $(MAKE_ROOT_DIR)/src/headers/sprites -iquote $(MAKE_ROOT_DIR)/src/headers/components -iquote $(MAKE_ROOT_DIR)/src/headers/fileio

# defines
CXX_FLAGS = --std=$(CXX_STANDARD) $(foreach def,$(shell cat build/config/defines.txt), -D$(def)) $(CXX_INCLUDES) $(shell sdl2-config --cflags)

# pattern rule for .cc files (C++ source)
%.o: %.cc
	@echo 'Compile $@ from $<'

# recipe to link the object files together into an executable
LINK.o = $(CC) $^ $(LDFLAGS) $(OUTPUT_OPTION) $(LDLIBS)

# linker flags
LDFLAGS = -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags
LDLIBS = -lSDL2 -lSDL2_image -llevelLoad -limage_class -lm -ltcl8.6 -ldl

# pattern rule for .d files (dependency files)
%.dep: %.cc
	@echo "Generate $@ from $<"
	@$(CXX) -E $(CXX_FLAGS) -MMD -MT $(addsuffix .o,$(notdir $*)) -MF $@ -o /dev/null $<

