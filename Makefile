TARGET := x86
MAKEFLAGS += -rR

ifeq ($(TARGET), x86)
	CC := gcc
	CFLAGS := -Dx86
else
	CC := x86_64-elf-gcc
	CFLAGS := -Dx64
endif

ifeq ($(BUILD), release)
	CFLAGS += -O3 -s
else
	CFLAGS += -O0
endif

CFLAGS += -x c

export TARGET CC CFLAGS

.PHONY: all src
all: bin src
	@echo "Build success"
	
bin:
	@mkdir -p $@
	
src:
	$(MAKE) -C ./$@

clean: src
	$(MAKE) -C ./$< clean
	@rm -rf ./bin