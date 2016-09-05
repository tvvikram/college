
sqleasy: tableMatrix.o genRecords.o main.o
	gcc -o ./sqleasy/sqleasy tableMatrix.o genRecords.o main.o
	mv ./sqleasy/sqleasy ./files/

tableMatrix.o:
	 flex ./sqleasy/createMatrix.l
	 mv lex.yy.c ./sqleasy/
	 gcc -o tableMatrix.o -c -g ./sqleasy/lex.yy.c

main.o:
	gcc -c -g ./sqleasy/main.c

genRecords.o:
	gcc -c -g ./sqleasy/genRecords.c

.PHONY: clean
clean:
	rm *.o
	rm *~
	rm lex.yy.c
