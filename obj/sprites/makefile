OBJFILES := $(shell cat $(CONFDIR)/obj_files_sprites.txt)

.PHONY: target

%.o: ../../$(SRCDIR)/sprites/%.cc
	$(GPP) $(DBG) -c -Wall $(CFLAGS) $< -o $@

target: $(OBJFILES)
	# 
	# 

