# search paths for files
vpath %.cc $(subst :, ,$(SRCPATH))
vpath %.o build/obj/main build/obj/sound build/obj/components
vpath %.d build/depend
vpath %.cc.o .
