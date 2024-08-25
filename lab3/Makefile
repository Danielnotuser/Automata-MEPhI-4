MEMCHECK = valgrind --leak-check=full
CODE = code.txt
CC = g++
CFLAGS = -g -std=c++20 -o
LEX = flex
YACC = bison
YFLAGS = -d -y -v -Wcounterexamples -Wno-yacc
BIN = lab
YFILE = prog.y
LFILE = prog.l
SRC = operation.cpp variable.cpp operand.cpp function.cpp
YOUT = y.tab.c y.tab.h
LOUT = lex.yy.c

.PHONY : all

all : $(BIN)

bison: $(YFILE)
	$(YACC) $(YFLAGS) $^

flex: $(LFILE)
	$(LEX) $^

build: $(LOUT) $(YOUT) $(SRC)
	$(CC) $^ $(CFLAGS) $(BIN)
		
$(BIN) : $(LOUT) $(YOUT) $(SRC)
	$(CC) $^ $(CFLAGS) $@

$(LOUT) : $(LFILE)
	$(LEX) $^

$(YOUT) : $(YFILE)
	$(YACC) $(YFLAGS) $^

clean:
	rm $(LOUT) $(YOUT) $(BIN)

