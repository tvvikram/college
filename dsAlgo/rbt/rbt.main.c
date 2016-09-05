#include<stdio.h>
#include<stdlib.h>

struct rbt;
typedef struct rbt * RBT;

RBT genRandRBT(RBT , int);
RBT autodelRoot(RBT, int);

int main(int argc, char **argv)
{
	RBT t = NULL, temp = NULL;
	int low = 10000, high = 100000, range = 10000, n;
	struct timeval startTime, endTime;
	double runTime;
	FILE *insp, *delp, *dotty;
	insp = fopen("rbt.Ins.dat", "w");
	delp = fopen("rbt.Del.dat", "w");
	dotty = fopen("rbt.dot", "w");
	fprintf(dotty, "digraph rbt {");
	
	if (argc >= 3)
	{	low = atoi(argv[1]);
		high = atoi(argv[2]);
		if (argc == 4)
			range = atoi(argv[3]);
	}
	
/*	if (DEBUG)
	{
		printf ("Debug mode...\n");
		printf ("Enter number of nodes: ");
		scanf ("%d", &high);
		t = genRandRBT (t, n);
		return 0;
	}*/
	printf("Low: %d, High: %d, Offset: %d\n", low, high, range);
	printf("Nodes Created | Time Taken\t  Deleted nodes out of nodes | Time taken\n");
	for (n = low; n < high; n = n + range)			// creates RBT with 'n' nodes and deletes it
  	{
		gettimeofday(&startTime, NULL);
		t = genRandRBT(t, n);
		gettimeofday(&endTime, NULL);
		runTime = (endTime.tv_sec * 1000 + endTime.tv_usec / 1000) - (startTime.tv_sec * 1000 + startTime.tv_usec / 1000);
		printf("    %d\t    %.0lf\t", n, runTime); 
		fprintf(insp,"%d\t%.0lf\n", n, runTime);
		
		if (n == low)
			dispInDotty(t, dotty);
		
		gettimeofday(&startTime, NULL);
		t = autodelRoot(t, n);
		gettimeofday(&endTime, NULL);
		runTime = (endTime.tv_sec * 1000 + endTime.tv_usec / 1000) - (startTime.tv_sec * 1000 + startTime.tv_usec / 1000);
		printf(" of %d\t  %.0lf\n", n, runTime);
		fprintf(delp, "\n%d\t%.0lf", n, runTime);		
	}
	fprintf(dotty, "}");
	fclose(insp);
	fclose(delp);
	return 0;
}
