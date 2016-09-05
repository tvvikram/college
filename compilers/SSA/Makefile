
ssa.out:  main.o genir.o dominator.main.o phi.o dfn.o rename.o
	gcc -o ssa.out genir.o dominator.main.o dfn.o phi.o rename.o main.o

dfn.o: dfn.c
	gcc -c -g dfn.c

dominator.main.o: dominator.main.c
	gcc -c -g dominator.main.c

main.o: main.c
	gcc -c -g main.c

genir.o: genir.c
	gcc -c -g genir.c

phi.o: phi.c
	gcc -c -g phi.c

rename.o: rename.c
	gcc -c -g rename.c

.PHONY: clean
clean:
	rm -f *.o *~ *.dot
