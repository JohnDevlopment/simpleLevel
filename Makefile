# disable implicit rules
.SUFFIXES:

# compiler
CC = g++

# search paths for dependencies
VPATH ::= build/obj/main:build/obj/sound:build/obj/components:build/obj/fileio:src:src/sound:src/components:src/fileio:build/depend

# target
TARGET ::= simpleLevel

# top-level root directory
MAKE_ROOT_DIR ::= $(shell pwd)

# build directory
#MAKE_BUILD_DIR = $(MAKE_ROOT_DIR)/build

# linker flags
LFLAGS = $(shell sdl2-config --libs) -lSDL2_image -llevelLoad -limage_class -lm -ltcl8.6 -ldl


# object file definitions #

# object files from /src
OBJ_MAIN ::= $(foreach file, $(shell cat build/config/obj_main.txt), $(file))

# object files from /src/sound
OBJ_SOUND = sound_music_fmt.o sound_ogg.o sound_wavstream.o

# object files /src/components
OBJ_COMPONENTS = pdcomponent.o pdobject.o

# object files from /src/fileio
OBJ_FILEIO = pdinifile.o log.o


# assembly files definitions #

# assembly files from /src
ASS_MAIN = $(addsuffix .s, $(basename $(OBJ_MAIN)))

# assembly files from /src/sound
ASS_SOUND = $(addsuffix .s, $(basename $(OBJ_SOUND)))

# assembly files from /src/components
ASS_PDOBJECT = $(addsuffix .s, $(basename $(OBJ_COMPONENTS)))

# assembly files from /src/fileio
ASS_FILEIO = $(addsuffix .s, $(basename $(OBJ_FILEIO)))


# dependency file definitions #

# dependency files from /src/fileio
DEP_FILEIO = $(addsuffix .d, $(basename $(OBJ_FILEIO)))


# let sub-make access these variables
export MAKE_ROOT_DIR VPATH DEPFILES

# all targets
.PHONY: default_target
default_target: all

# default target: compiles all object files and links them together to the binary executable
all: exec/$(TARGET)
	@echo "Built $<"

# link object files to make the binary executable
.PHONY: exec/$(TARGET)
exec/$(TARGET): $(OBJ_MAIN) $(OBJ_SOUND) $(OBJ_COMPONENTS)
	@echo "Linking object files to make $@"
	@echo $(CC) -o $@ $^ $(LFLAGS)

# print a help message
.PHONY: help
help:
	@echo "List of targets:"
	@echo "... all (default if no target is given)"
	@for file in $(basename $(OBJ_MAIN)); do \
		echo "... $${file}.o\n... $${file}.s"; \
		done
	@$(MAKE) -f $(lastword $(MAKEFILE_LIST)) --no-print-directory help-sound
	@$(MAKE) -f $(lastword $(MAKEFILE_LIST)) --no-print-directory help-pdobject
	@echo "... $(shell tput bold)help$(shell tput sgr0) (this help message)"

.PHONY: help-sound
help-sound:
	@for file in $(basename $(OBJ_SOUND)); do \
	echo "... $${file}.o\n... $${file}.s"; \
	done

.PHONY: help-pdobject
help-pdobject:
	@for file in $(basename $(OBJ_COMPONENTS)); do \
	echo "... $${file}.o\n... $${file}.s"; \
	done

# string that is compiled for different targets
%.o: string_compilethis = "$(shell tput bold)Compiling$(shell tput sgr0) $@"
%.s: string_compilethis = "$(shell tput bold)Outputting assembly file$(shell tput sgr0) $@"
%.d: string_compilethis = "$(shell tput bold)Generating dependency file$(shell tput sgr0) $@"

# each object file is a target
$(OBJ_MAIN): %.o: %.cc %.d
	@echo $(string_compilethis)
	@$(MAKE) -r -f build/build.make build/obj/main/$@

$(OBJ_SOUND): %.o: %.cc %.d
	@echo $(string_compilethis)
	@$(MAKE) -f build/build.make build/obj/sound/$@

$(OBJ_COMPONENTS): %.o: %.cc %.d
	@echo $(string_compilethis)
	@$(MAKE) -f build/build.make build/obj/components/$@

$(OBJ_FILEIO): DEPFILES ::= $(OBJ_FILEIO:.o=.d)
$(OBJ_FILEIO): %.o: %.cc %.d
	@echo $(string_compilethis)
	@$(MAKE) -r -f build/build.make build/obj/fileio/$@

# assembly files
$(ASS_MAIN): %.s: %.cc
	@echo $(string_compilethis)
	@$(MAKE) -f build/build.make build/obj/main/$@

$(ASS_SOUND): %.s: %.cc
	@echo $(string_compilethis)
	@$(MAKE) -f build/build.make build/obj/sound/$@

$(ASS_PDOBJECT): %.s: %.cc
	@echo $(string_compilethis)
	@$(MAKE) -f build/build.make build/obj/components/$@

$(ASS_FILEIO): %.s: %.cc
	@echo $(string_compilethis)
	@$(MAKE) -f build/build.make build/obj/fileio/$@

# depends
%.d: %.cc
	@echo $(string_compilethis)
	@$(MAKE) -i -f build/depend.make build/depend/$@

