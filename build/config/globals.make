# macro for cat'ing a file to form it into a list of files; 1 = file
list-from-file = $(strip $(foreach file,$(shell cat $(1)), $(file)))

# bold text outputter; 1 = bold text, 2 = regular text
out-bold-text = @echo "$(shell tput bold)$(1)$(shell tput sgr0)" "$(2)"

# search for source file; 1 = list of directories; 2 = file to search for
search-files = $(firstword $(wildcard $(addsuffix /$(2),$(1))))

# remove the directory prefix and remove the extension, from a file name
file-stub = $(notdir $(basename $(1)))

# source paths
SRCPATH ::= src:src/components:src/sound

# remove command
RM ::= rm -fv
