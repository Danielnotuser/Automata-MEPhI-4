CPP_SRC = ./cpp/regex.cpp
FLEX_SRC = ./flex/scan.l
TESTS = ./1.txt

cpp: $(CPP_SRC)
	gcc $(CPP_SRC) -o lab
	
flex: $(FLEX_SRC) $(TESTS)
	flex $(FLEX_SRC)
	gcc lex.yy.c -o scan
	./scan $(TESTS)
