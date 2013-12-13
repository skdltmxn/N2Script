OBJ := $(patsubst %.c, %.o, $(wildcard *.c))
OBJ += lex.o grammar.o
BISON := bison
FLEX := flex
DEPFOLDER := .deps
OUTPUT := N2Script
DEPS := $(wildcard $(DEPFOLDER)/*.d)

.PHONY: all deps clean
all: deps link
	@mv $(OUTPUT) ../bin/
	
link: $(OBJ)
	$(CC) -o $(OUTPUT) $^

deps:
	@mkdir -p $(DEPFOLDER)
	
%.o: %.c Makefile
	$(CC) $(CFLAGS) -MD -MF $(DEPFOLDER)/$*.d -c -o $@ $<

lex.c: lex.l grammar.h
	$(FLEX) -o $@ $<

grammar.h: grammar.c

grammar.c: grammar.y
	$(BISON) -d -o $@ $<
	
clean:
	@rm -rf ./$(DEPFOLDER)
	@rm -rf ./*.o
	
-include $(DEPS)