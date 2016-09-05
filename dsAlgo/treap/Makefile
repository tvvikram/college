treap.out:  treap.main.o treap.autoInsDel.o
	gcc -o treap.out -ggdb treap.main.o treap.autoInsDel.o

treap.genOp.o:
	gcc -c -ggdb treap.genOp.c

treap.main.o:
	gcc -c -ggdb treap.main.c

treap.insDel.o:
	gcc -c -ggdb treap.insDel.c

treap.autoInsDel.o:
	gcc -c -ggdb treap.autoInsDel.c

.PHONY: clean
clean:
	rm -f *.o *~ *.dot
