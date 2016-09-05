#include<stdio.h>
#include<stdlib.h>
#include "header.h"

int main(int argc, char **argv)
{
	char *filename;
	FILE *fp = fopen("insert.db", "w");	// file pointer
	int n, i, j;		// indices
	struct timeval startTime, endTime;
	double runTime;
		
	// initializing the matrix to -1
	for (i = 0; i < NO_OF_TABLES; i++)
		for (j = 0; j < NO_OF_ATTRIBUTES; j++)
			a[i][j] = -1;
	
	if (argc != 3)
	{
		printf("Usage: ./sqleasy `filename` `numberOfTuples`\n");
		exit(1);
	}

	filename = argv[1];
	n = atoi(argv[2]);

	createTableMatrix(filename);
	
	// calling genRecords() to generate 'n' records
	for (i = 0; i < NO_OF_TABLES && a[i][0] != -1; i++)
	{
		gettimeofday(&startTime, NULL);
		genRecords(n, i, fp);
		gettimeofday(&endTime, NULL);
		runTime = (endTime.tv_sec * 1000 + endTime.tv_usec / 1000) - (startTime.tv_sec * 1000 + startTime.tv_usec / 1000);
		printf("Generated %d records for table %s in %lf ms.\n", n, tableNames[i], runTime);
	}
	return 0;
}
