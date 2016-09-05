#include<stdio.h>
#include<stdlib.h>
//#include"dot.c"

struct treap;
typedef struct treap * TREAP;

TREAP genRandTreap(TREAP , int);
TREAP autodelRoot(TREAP, int);
void dispInDotty(TREAP, FILE *);

/*void menu()
{
  printf("\n0. Menu 1. Insert 2. Delete 3. Display\n");
}

int main()
{
  int choice, l = 0, key;
  TREAP t = NULL;
  do
  {
    printf("\nEnter choice: ");
    scanf("%d", &choice);
    switch(choice)
    {
	case 0: menu();				break;
	case 1: t = (TREAP)insert(t, getnode());	break;
	case 2: printf("\nEnter key to delete: ");
		scanf("%d", &key);
		t = (TREAP)delete(t, search(t, key));	break;
	case 3: dispTree(t);			break;
    }
  }
  while (l == 0);
  return 0;
}*/

int main(int argc, char **argv)
{
	TREAP t = NULL, temp = NULL;
	int low = 10000, high = 100000, range = 10000, n;
	struct timeval startTime, endTime;
	double runTime;
	FILE *insp, *delp, *dotty;
	insp = fopen("treap.Ins.dat", "w");
	delp = fopen("treap.Del.dat", "w");
	dotty = fopen("treap.dot", "w");
	fprintf(dotty, "digraph treap {");

	if (argc >= 3)
	{	low = atoi(argv[1]);
		high = atoi(argv[2]);
		if (argc == 4)
			range = atoi(argv[3]);
	}
	
	printf("Low: %d, High: %d, Offset: %d\n", low, high, range);
	printf("Nodes Created | Time Taken\tDeleted  nodes  out  of  nodes | Time taken\n"); 
	for (n = low; n < high; n = n + range)			// creates treap with 'n' nodes and deletes it
  	{
		gettimeofday(&startTime, NULL);
		t = genRandTreap(t, n);
		gettimeofday(&endTime, NULL);
		runTime = (endTime.tv_sec * 1000 + endTime.tv_usec / 1000) - (startTime.tv_sec * 1000 + startTime.tv_usec / 1000);
		printf("    %d\t    %.0lf  ", n, runTime); 
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
