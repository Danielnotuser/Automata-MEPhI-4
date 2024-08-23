MEMCHECK = valgrind --leak-check=full
CODE = code.txt
CC = gcc
CFLAGS = -g -o
LEX = flex
YACC = bison
YFLAGS = -d -y
BIN = lab
YFILE = prog.y
LFILE = prog.l
SRC = node.cpp variable.cpp operand.cpp
YOUT = y.tab.c y.tab.h
LOUT = lex.yy.c

.PHONY : all

all : $(BIN)
	$(MEMCHECK) ./$(BIN) $(CODE)

$(BIN) : $(LOUT) $(YOUT) $(SRC)
	$(CC) $^ $(CFLAGS) $@

$(LOUT) : $(LFILE)
	$(LEX) $^

$(YOUT) : $(YFILE)
	$(YACC) $(YFLAGS) $^

