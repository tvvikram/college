#include<stdio.h>
#include<stdlib.h>
#include "header.h"

// to display DF contents
int dispDFn()	{

	int i, j;	// index
	printf("\nDFn:\n");
	
	for (i = 0; i < NO_OF_NODES; i++)	{
		for (j = 0; j < NO_OF_NODES; j++)
			printf("%d\t", DF[i][j]);
		printf("\n");
	}
	return 0;
}

// to compute the DFn for each node, starting from the root node
void computeDFn(int n)	{

	int S[NO_OF_NODES] = {}, i = 0;	// storage array and its index 'i'
	int y, c, w, j;		// indices
	
	for (i = 0; i < NO_OF_NODES; i++)
		S[i] = -1;	// initialize the storage array
	i  = 0;			// resetting storage index
	
	// to compute DFlocal[n]:
	// for each node 'y' in successor of 'n'
	for (y = 0; y < NO_OF_NODES; y++)	{
		// if 'n' is not the immediately dominate 'y', add 'y' to storage array
		if (y != 0 && cfg[n][y] == 1 && idominator[y] != n)
			S[i++] = y;
	}
	
	// for each child 'c' of 'n' in the dominator tree
	for (c = 0; c < NO_OF_NODES; c++)	{	
		if (idominator[c] == n)	{	// checking if 'n' is parent of 'c'
		
			// recursively call to compute DFn at node 'c'
			computeDFn(c);
			
			// to compute DFup[c]
			for (j = 0; j < NO_OF_NODES; j++)	{
				// check for an entry in DF[c]
				if (DF[c][j] != -1)	{
				
					// for each element 'w' of DF[c]
					w = DF[c][j];
					// if 'n' does not dominate 'w' or if 'n' equals 'w',
					// add 'w' to storage array
					if(idominator[w] != n || n == w)
						S[i++] = w;
				}
			}
		}
	}
	
	// finally copy the storage array contents to DF[n]
	for (j = 0; j < i; j++)
		DF[n][j] = S[j];
}
