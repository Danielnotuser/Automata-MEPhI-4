MEMCHECK = valgrind --leak-check=full
CODE = code.txt
CC = g++
CFLAGS = -g -std=c++20 -o
LEX = flex
YACC = bison
YFLAGS = -d -y -Wno-yacc
BIN = lab
YFILE = prog.yy
LFILE = prog.l
SRC = node.cpp variable.cpp operand.cpp
YOUT = y.tab.c y.tab.h
LOUT = lex.yy.cc

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
	rm lex.* y.tab* location.hh lab

