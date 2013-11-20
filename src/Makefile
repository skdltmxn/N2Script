OBJ := $(patsubst %.c, %.o, $(wildcard *.c))
DEPFOLDER := .deps
OUTPUT := N2Script

%.o: %.c
	$(CC) $(CFLAGS) -MD -MF $(DEPFOLDER)/$*.d -c -o $@ $<

.PHONY: all deps clean
all: deps link
	@mv $(OUTPUT) ../bin/
	
link: $(OBJ)
	$(CC) -o $(OUTPUT) $<

deps:
	@mkdir -p $(DEPFOLDER)
	
	
clean:
	@rm -rf ./*.o