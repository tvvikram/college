mvh:	main.c mvh.o dmvh.o
	gcc -ggdb -lm -o mvh main.c mvh.o dmvh.o

mvh.o:	mvh.c mvh.h
	gcc -ggdb -lm -c mvh.c 

dmvh.o:	dmvh.c dmvh.h
	gcc -ggdb -lm -c dmvh.c
	
clean:
	rm -f *.o mvh *~ *.dot
