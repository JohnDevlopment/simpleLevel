OBJFILES := $(shell cat $(CONFDIR)/obj_files_sound.txt)

.PHONY: target

%.o: ../../$(SRCDIR)/sound/%.cc
	$(GPP) $(DBG) -c -Wall $(CFLAGS) $< -o $@

target: $(OBJFILES)
	# 
	# 
