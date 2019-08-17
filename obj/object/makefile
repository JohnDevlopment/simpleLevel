OBJFILES := $(shell cat $(CONFDIR)/obj_files_object.txt)

.PHONY: target

%.o: ../../$(SRCDIR)/object/%.cc
	$(GPP) $(DBG) -c -Wall $(CFLAGS) $< -o $@

target: $(OBJFILES)
	# 
	# 

