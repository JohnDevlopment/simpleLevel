# build script
include build/config/globals.make
include build/config/vpath.make
include build/config/pat.make

.PHONY: default_target
default_target:

include build/depend/main.make


