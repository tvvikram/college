
	/* Program to 'quick' sort the elements */

#include<iostream>
#include<stdlib.h>
#include<sys/time.h>
#include"sortutil.h"


#define RANGELOW  500000
#define RANGEHIGH 10000000

const int OFFSET = 500000;

int main()
{
	SORT quick;
	double runTime;
	struct timeval startTime, endTime;
	FILE *fp;
	fp = fopen("quick.dat", "w");
	
	for (int i = RANGELOW; i < RANGEHIGH; i = i + OFFSET)
	{
		quick.genRand(OFFSET);		/* call to generate random numbers */
		
		int *a = quick.getarray(), length = quick.getlength();
	//	printArray(quick);		/* Release comments to get array printed */


		gettimeofday(&startTime, NULL);
		quick.quickSort(0, length - 1, a);		/* call to quick sort the elements */
		gettimeofday(&endTime, NULL);

		runTime = (endTime.tv_sec * 1000 + endTime.tv_usec / 1000) - (startTime.tv_sec * 1000 + startTime.tv_usec / 1000);
	//	printArray(quick);

		if (!quick.checkSort())	/* success of sort */
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
