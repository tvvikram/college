a.out: lex.yy.c y.tab.c Makefile
	gcc -ggdb lex.yy.c y.tab.c -ll -ly

y.tab.c: parse.y Makefile
	yacc -dvg parse.y

lex.yy.c: lex.l Makefile
	flex lex.l

graph: y.dot
	kgraphviewer y.dot &

.PHONY: clean
clean:
	rm -f *.o *.out *~ lex.yy.c y.tab.c y.tab.h y.output y.dot
