betaparse: y.tab.c lex.yy.c genDotty.c
	gcc -o betaparse -ggdb y.tab.c lex.yy.c genDotty.c

y.tab.c: parse.y
	yacc -d parse.y

lex.yy.c: lex.l
	lex -ll lex.l


.PHONY: clean
clean:
	clear
	rm -f lex.yy.c y.tab.c y.tab.h betaparse
	rm -f *.o *~ *.out *.dot *.html

.PHONY: install
install:
	./install.sh

tree: tree.dot
	dotty tree.dot

view: tree.html
	firefox tree.html

uninstall:
	sudo rm /bin/betaparse
