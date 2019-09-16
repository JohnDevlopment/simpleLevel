# makefile with build rules
include build/config/flags.make

.PHONY: clean
clean:
	@echo "Clean project"

compile-obj-file = @$(CC) $(CXX_FLAGS) -c -o $@ $<

# /src
build/obj/main/%.o: %.cc | build/obj build/obj/main
	$(compile-obj-file)

build/obj/main/%.s: src/%.cc | build/obj build/obj/main
	@$(CC) $(CXX_FLAGS) -S $< -o $@

# /src/sound
build/obj/sound/%.o: src/sound/%.cc | build/obj build/obj/sound
	@$(CC) $(CXX_FLAGS) -c $< -o $@

build/obj/sound/%.o: src/sound/%.cc | build/obj build/obj/sound
	@$(CC) $(CXX_FLAGS) -S $< -o $@

# /src/components
build/obj/components/%.o: src/components/%.cc | build/obj build/obj/components
	@$(CC) $(CXX_FLAGS) -c -o $@ $<

build/obj/components/%.s: src/components/%.cc | build/obj build/obj/components
	@$(CC) $(CXX_FLAGS) -S -o $@ $<

# /src/fileio
#build/obj/fileio/%.o: src/fileio/%.cc | build/obj build/obj/fileio
build/obj/fileio/%.o: %.cc | build/obj build/obj/fileio
	@$(CC) $(CXX_FLAGS) -c -o $@ $<

build/obj/fileio/%.s: src/fileio/%.cc | build/obj build/obj/fileio
	@$(CC) $(CXX_FLAGS) -S -o $@ $<

# make directories if they don't already exist
build/obj:
	@mkdir $@
	@echo "Created directory: $@"

build/obj/main:
	@mkdir $@
	@echo "Created directory: $@"

build/obj/components:
	@mkdir $@
	@echo "Created directory: $@"

build/obj/fileio:
	@mkdir $@
	@echo "Created directory: $@"

-include $(DEPFILES)
