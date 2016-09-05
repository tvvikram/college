#include<stdio.h>
#include<stdlib.h>
#include "header.h"

// contains the places (nodes) of definition of the variables
int defSites[NO_OF_VARIABLES][NO_OF_NODES];

// to display aPHI contents
int dispAPhi()	{

	int i, j;	// indices
	printf("\naPHI:");
	for (i = 0; i < NO_OF_VARIABLES; i++)	{
		printf("\n");
		for (j = 0; j < NO_OF_NODES; j++)
			printf("%d\t", aPHI[i][j]);
	}
	return 0;
}

// to display defSites contents
int dispDefSites()	{

	int i, j;	// indices
	printf("defSites:");	
	for (i = 0; i < NO_OF_VARIABLES; i++)	{
		printf("\n");
		for (j = 0; j < NO_OF_NODES; j++)
			printf("%d\t", defSites[i][j]);
	}
	return 0;
}

// to place phi functions for the generated IR - SSA intermediate FORM
void placePHIfunctions()	{

	int a;							// a variable
	int i, j, k;					// indices
	int workList[NO_OF_NODES * 2];	// a worklist
	
	for (i = 0; i < NO_OF_VARIABLES; i++)
		for (j = 0; j < NO_OF_NODES; j++)
			defSites[i][j] = -1;	// initialize defSites to -1
	
	// for each variable find the nodes of their definition using aOrig and load them to defSites
	for (i = 0; i < NO_OF_NODES; i++)	{
		for (j = 0; aOrig[i][j] != -1; j++)	{
			int x = aOrig[i][j];
			for (k = 0; defSites[x][k] != -1; k++);    // traverse till the end for each variable
			defSites[x][k] = i;
		}
	}

	// for each variable 'a'			
	for (a = 0; a < NO_OF_VARIABLES; a++)	{
		// clear workList to -1
		for (i = 0; i < NO_OF_NODES * 2; i++)
			workList[i] = -1;
		// load worklist with defined sites of variable 'a'
		for (i = 0; defSites[a][i] != -1; i++)
			workList[i] = defSites[a][i];

		// for each element 'node' of worklist, until the worklist becomes empty
		for (j = 0; j < i; j++)	{
			int node = workList[j], m, Y;
			// if there exists a dominance frontier for the 'node'
			for (m = 0; m < NO_OF_NODES && DF[node][m] != -1; m++)	{
				int flag = 1;				// false value
				// load the frontier to 'Y'
				Y = DF[node][m];
				// check if the variable 'a' already belongs to aPHI[Y]
				for (k = 0; k < NO_OF_VARIABLES && aPHI[Y][k] != -1; k++)	{
					if (a == aPHI[Y][k])	{
						flag = 0;			// if variable found in aPHI[Y], true value
						k = NO_OF_VARIABLES;
					}
				}
				// if variable 'a' not found in aPHI[Y]
				if (flag)	{
					int l;					
					// then insert the 'phi' statement for variable 'a' at the begining
					// of node 'Y'
					for (l = 0; l < NO_OF_NODES; l++)	{	
						// check for the predecessors of 'Y' using cfg
						if (cfg[l][Y] == 1)
							// increment the value of 0th column for variable 'a' of block 'Y'
							// indicating the number of arguments to phi fucntion
							// (number of arguments = number of predecessors)
							aPHIstatements[Y][a][0] = aPHIstatements[Y][a][0] + 1;
					}
					// now add the variable to aPHI[Y] list
					for (k = 0; aPHI[Y][k] != -1; k++);
					aPHI[Y][k] = a;
					flag = 1;		// reset flag
					// check if the variable 'a' already belongs to aOrig[Y]
					for (k = 0; k < NO_OF_VARIABLES && aOrig[Y][k] != -1 ; k++)	{
						if (a == aOrig[Y][k])	{
							flag = 0;		// if variable found in aOrig[Y], true value
							k = NO_OF_VARIABLES;
						}
					}
					// if variable 'a' not found in aOrig[Y]
					if (flag)
						workList[i++] = Y;			// then add 'Y' to worklist
				}
			}
		}
	}
}
