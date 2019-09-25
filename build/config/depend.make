# disable built-in suffix rules
.SUFFIXES:

# global variables
include build/config/globals.make

# search paths for dependencies
include build/config/vpath.make

# pattern rules
include build/config/pat.make

# default goal if none is specified
.PHONY: default_target
default_target:
	$(error $@ invalid target; also cannot call make with no target)

# make directory if neccessary
build/depend:
	@mkdir -p build/depend
	@echo "Created directory: $(abspath build/depend)"

# target 'depend/main' routes to target 'build/depend/main.make'
.PHONY: depend/main
depend/main: build/depend/main.make

# this target reads from and DEPENDS on /tmp/depend_main.txt
build/depend/main.make: /tmp/depend_main.txt | build/depend
	echo > $@
	sed 's,\(^.*\)\.o[ :],build/obj/main/\1.o :,g' $< > $@

# temp file with all rules inside
/tmp/depend_main.txt: $(addsuffix .dep,$(basename $(call list-from-file,build/config/obj_main.txt)))
	echo > $@
	for file in $^; do \
		echo "`cat $$file`\n" >> $@; \
	done
	$(RM) $^

# do not try to build these targets
$(wildcard build/config/*.make): ;

