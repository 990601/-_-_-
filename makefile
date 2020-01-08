result:parser.y lex.l def.h
	bison -d  parser.y
	flex lex.l
	gcc parser.tab.c lex.yy.c ast.c analyse.c aim.c

.PHONY: clean
clean:
	rm a.out parser.tab.c parser.tab.h lex.yy.c
