CC = gcc
CFLAGS = -o
LEX = flex
YACC = bison
YFLAGS = -d -y
BIN = lab
YFILE = prog.y
LFILE = prog.l
NFILE = node.c
YOUT = y.tab.c y.tab.h
LOUT = lex.yy.c

.PHONY : all

all : $(BIN)
	./$(BIN)

$(BIN) : $(LOUT) $(YOUT) $(NFILE)
	$(CC) $^ $(CFLAGS) $@

$(LOUT) : $(LFILE)
	$(LEX) $^

$(YOUT) : $(YFILE)
	$(YACC) $(YFLAGS) $^

