TARGET	=  simpleLevel
DEFINES	=  $(foreach def,$(shell cat $(CONFDIR)/defines),-D$(def))
STD	=  c++11
CURDIR	=  $(shell pwd)
INCDIR	=  -I/usr/include/tcl8.6 -I/usr/local/include/custom -iquote $(CURDIR)/$(SRCDIR)/headers -iquote $(CURDIR)/$(SRCDIR)/headers/sprites
CFLAGS	=  $(shell sdl2-config --cflags) --std=$(STD) $(DEFINES) $(INCDIR)
LIBS	=  $(shell sdl2-config --libs) -lSDL2_image -llevelLoad -limage_class -lSDL2_ttf -lm -ltcl8.6 -ldl
DBG	=  -g

BINDIR	:= bin
SRCDIR	:= src
OBJDIR	:= obj
LIBDIR	:= libs
CONFDIR	:= $(CURDIR)/config
GPP	:= g++

export GPP CONFDIR CFLAGS STD DEFINES OBJDIR SRCDIR DBG

$(BINDIR)/$(TARGET): compile
	$(GPP) $(foreach file,$(shell cat $(CONFDIR)/obj_files.txt),$(OBJDIR)/$(file)) $(foreach file,$(shell cat $(CONFDIR)/lib_files.txt),$(LIBDIR)/$(file)) $(foreach file,$(shell cat $(CONFDIR)/obj_files_sound.txt),$(OBJDIR)/sound/$(file)) $(foreach file,$(shell cat $(CONFDIR)/obj_files_object.txt),$(OBJDIR)/object/$(file)) $(foreach file,$(shell cat $(CONFDIR)/obj_files_sprites.txt),$(OBJDIR)/sprites/$(file)) -o $@ $(LIBS)

.PHONY: clean compile

# cleans the entire workspace
clean:
	-rm --verbose $(BINDIR)/$(TARGET)
	-rm --verbose $(foreach file,$(shell cat $(CONFDIR)/obj_files.txt),$(OBJDIR)/$(file))
	-rm --verbose $(foreach file,$(shell cat $(CONFDIR)/obj_files_sound.txt),$(OBJDIR)/sound/$(file))
	-rm --verbose $(foreach file,$(shell cat $(CONFDIR)/obj_files_sprites.txt),$(OBJDIR)/sprites/$(file))
	-rm --verbose $(foreach file,$(shell cat $(CONFDIR)/obj_files_object.txt),$(OBJDIR)/object/$(file))

# compiles the source files into object files
compile:
	@$(MAKE) -C $(OBJDIR) target
	@$(MAKE) -C $(OBJDIR)/sound target
	@$(MAKE) -C $(OBJDIR)/object target
	@$(MAKE) -C $(OBJDIR)/sprites target
