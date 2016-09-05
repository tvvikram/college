a.out: parse.c lex.c symbolTable.h token.h
	gcc -ggdb parse.c

test:
	./a.out test$(ARGS).cs
	dotty dotty.dot

.PHONY clean:
clean:
	rm -f *~ *.o *.out
