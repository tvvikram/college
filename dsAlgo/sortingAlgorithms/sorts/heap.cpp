
	/* Program to 'heap' sort the elements */

#include<iostream>
#include<stdlib.h>
#include<sys/time.h>
#include"sortutil.h"


#define RANGELOW  100000
#define RANGEHIGH 2000000

const int OFFSET = 100000; 

int main()
{
	SORT heap;
	double runTime;
	struct timeval startTime, endTime;
	FILE *fp;
	fp = fopen("heap.dat", "w");
	
	for (int i = RANGELOW; i < RANGEHIGH; i = i + OFFSET)
	{
		heap.genRand(OFFSET);		/* call to generate random numbers */
		
		int *a = heap.getarray(), length = heap.getlength();
	//	printArray(heap);		/* Release comments to get array printed */


		gettimeofday(&startTime, NULL);
		heap.heapSort(length - 1, a);		/* call to heap sort the elements */
		gettimeofday(&endTime, NULL);

		runTime = (endTime.tv_sec * 1000 + endTime.tv_usec / 1000) - (startTime.tv_sec * 1000 + startTime.tv_usec / 1000);
	//	printArray(heap);

		if (!heap.checkSort())	/* success of sort */
		{
	//		cout << endl << "Non - descendingly sorted." << endl;
			printf("\n%d\t%.0lf", i, runTime);		/* Non - descendingly sorted */
			fprintf(fp,"\n%d\t%.0lf", i, runTime);
		}
		else
			printf("\nNot non - descendingly sorted.");
	}
	printf("\n");
	return 0;
}
