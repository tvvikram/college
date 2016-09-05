bplus:
	make clean
#	flex lex.l
	gcc -ggdb -o bplus.out bplus.main.c # lex.yy.c
	#make bplus.out
	#mv bplus.out a.out

bplus.out: bplus.main.o
	gcc -ggdb -o bplus.out bplus.main.o

bplus.main.o:
	gcc -c -ggdb bplus.main.c

bplus.records.o:
	gcc -c -ggdb bplus.records.h

graph:
	kgraphviewer BP0.dot

file:
	cat inputSaved > inputArgs

.PHONY: clean
clean:
	clear
	rm -f *.out *~ *.o *.ps *.dot
