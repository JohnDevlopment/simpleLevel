# generate dependencies for the selected target
include build/config/flags.make

TMPFILE ::= /tmp/simpleLevel.tmp
DEPFLAGS = -MT $@ -MMD -MP -MF $(TMPFILE)
DEPTARGET = build/depend/$*.d
BASETARGET = $(basename $@)

build/depend/%.d: %.cc | build/depend
	@echo "Logging headers for $<"
	@$(CC) $(CXX_FLAGS) $(DEPFLAGS) -c $< -o /dev/null; \
	sed 's,\($*\)\.o[ :]*,\1.o $*.d : ,g' < $(TMPFILE) > $@; \
	rm -fv $(TMPFILE)

build/depend:
	@mkdir -p $@
	@echo "Created directory: $@"

