
dominators : dominator.o gencfg.o main.o
	gcc -o dominators dominator.o main.o gencfg.o

dominator.o : dominator.c header.h
	gcc -c -ggdb dominator.c
main.o : main.c header.h
	gcc -c -ggdb main.c
gencfg.o : gencfg.c header.h
	gcc -c -ggdb gencfg.c

.PHONY: clean
clean:
	rm -f *~ *.out *.o *.dot
