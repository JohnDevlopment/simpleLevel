OBJFILES := $(shell cat $(CONFDIR)/obj_files.txt)

.PHONY: target

%.o: ../$(SRCDIR)/%.cc
	$(GPP) $(DBG) -c -Wall $(CFLAGS) $< -o $@

target: $(OBJFILES)
	# 
	# 

