
	/* Program to 'bubble' sort the elements */

#include<iostream>
#include<stdlib.h>
#include<sys/time.h>
#include"sortutil.h"


#define RANGELOW  10000
#define RANGEHIGH 100000

const int OFFSET = 10000; 

int main()
{
	SORT bubble;
	double runTime;
	struct timeval startTime, endTime;
	FILE *fp;
	fp = fopen("bubble.dat", "w");
	
	for (int i = RANGELOW; i < RANGEHIGH; i = i + OFFSET)
	{
		bubble.genRand(OFFSET);		/* call to generate random numbers */
		
		int *a = bubble.getarray(), length = bubble.getlength();
	//	printArray(bubble);		/* Release comments to get array printed */


		gettimeofday(&startTime, NULL);
		bubble.bubbleSort(length - 1, a);		/* call to bubble sort the elements */
		gettimeofday(&endTime, NULL);

		runTime = (endTime.tv_sec * 1000 + endTime.tv_usec / 1000) - (startTime.tv_sec * 1000 + startTime.tv_usec / 1000);
	//	printArray(bubble);

		if (!bubble.checkSort())	/* success of sort */
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
