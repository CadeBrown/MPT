CFLAGS           ?= -Ofast -fopenmp -std=c99
LDFLAGS          ?= 

# -*- FILES -*-

# general purpose headers
all_H            := $(wildcard include/*.h)

# nttcript library
MPT_C            := src/MPT.c src/util.c src/arith.c

# -*- TARGETS -*-

# target files
MPT_BIN          := MPT

# generated
MPT_O            := $(patsubst %.c,%.o,$(MPT_C))


# -*- RULES -*-

.PHONY: all default clean FORCE

# default target to build
default: $(MPT_BIN)

# build everything
all: default

clean: FORCE
	rm -rf $(wildcard $(MPT_O) $(MPT_BIN) build bin)


# target to force another target
FORCE:

# rule to compile a single object file
src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DMPT_BUILD -I./include/ $< -fPIC -c -o $@

# rule to build the executable (no extension) from the library and it's `.o`'s
#   since we require a library, and object files, we don't use `$^`, but just build
#   explicitly
$(MPT_BIN): $(MPT_O)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(MPT_O) $(LDFLAGS) \
		-lm \
		-o $@
	strip $@ $(STRIP_OPTS)
	