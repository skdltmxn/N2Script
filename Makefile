CC ?= gcc
TARGET ?= $(shell uname -m | sed -e s/i.86/x86/ -e s/x86_64/x64/)
MAKEFLAGS += -rR --no-print-directory

ifeq ($(TARGET), x86)
	CFLAGS := -Dx86
else
	CFLAGS := -Dx64
endif

ifeq ($(BUILD), release)
	CFLAGS += -O -s
else
	CFLAGS += -O0
endif

CFLAGS += -x c

export TARGET CC CFLAGS

.PHONY: all src clean
all: bin src
	@echo "Build success"
	
bin:
	@mkdir -p $@
	
src:
	$(MAKE) -C ./$@

clean:
	$(MAKE) -C src clean
	@rm -rf ./bin