
sqleasy: tableMatrix.o genRecords.o main.o
	gcc -o sqleasy tableMatrix.o genRecords.o main.o
	mv sqleasy ../files/

tableMatrix.o:
	 flex createMatrix.l
	 gcc -o tableMatrix.o -c -g lex.yy.c

main.o:
	gcc -c -g main.c

genRecords.o:
	gcc -c -g genRecords.c

.PHONY: clean
clean:
	rm *.o
	rm *~
	rm lex.yy.c
