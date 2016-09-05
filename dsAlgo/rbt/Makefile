
rbt.out:  rbt.main.o rbt.autoInsDel.o
	gcc -o rbt.out rbt.main.o rbt.autoInsDel.o

rbt.genOp.o:
	gcc -c rbt.genOp.c

rbt.main.o:
	gcc -c rbt.main.c

rbt.insDel.o:
	gcc -c rbt.insDel.c

rbt.autoInsDel.o:
	gcc -c rbt.autoInsDel.c

.PHONY: clean
clean:
	rm -f *.o *.out *~
