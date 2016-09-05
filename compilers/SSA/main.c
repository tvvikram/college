#include<stdio.h>
#include<stdlib.h>
#include "header.h"

// writing to file in dotty format
void dispInDotty(FILE *fp, char *name)	{

	int i, j, k;	// indices
	int flag = 1;	// 0 - true value for flag
	fprintf(fp, "digraph %s {\n", name);
	
	// for each node
	for (i = 0; i < NO_OF_NODES; i++)	{
		int flag = 1;
		// check if the basicBlock contains any entry
		for (j = 1; j <= basicBlock[i][0]; j++)
			if (basicBlock[i][j] != -1)	{
				flag = 0;		// if yes, change the flag value
				j = basicBlock[i][0] + 1;
			}
		// check for phi placements if node is empty
		if (flag == 1)	{
			for (j = 0; j < NO_OF_VARIABLES; j++)
				// check for phi entries
				if (aPHIstatements[i][j][0] > 0)	{
					flag = 0;	// change the flag value
					j = NO_OF_VARIABLES;
				}
		}
		// if there are basicBlock or phi entries
		if (!flag)	{
			// print block number, definitions in that block from aOrig[i] and DF numbers of
			// that particular block
			fprintf(fp, "%d [shape=box label=\"Block No: %d Defs: ", i, i);
			for (j = 0; aOrig[i][j] != -1; j++)
				fprintf(fp, "%d ", aOrig[i][j]);
			fprintf(fp, "\\n");
			fprintf(fp, " DFn's: ");
			for (j = 0; DF[i][j] != -1; j++)
				fprintf(fp, "%d ", DF[i][j]);
			fprintf(fp, "\\n");
			
			// for each variable in the node
			for (j = 0; j < NO_OF_VARIABLES; j++)	{
				// checking if there are phi entries
				if (aPHIstatements[i][j][0] > 0)	{
					// prints renamed variable-leftarrow-phi-bracket
					fprintf(fp, " t%d(%d) <- phi (", j, aPHIstatements[i][j][1]);
					// prints renamed variables aPHIstatements[i][j][0] times
					for (k = 0; k < aPHIstatements[i][j][0]; k++)
						fprintf(fp, "t%d(%d) ", j, aPHIstatements[i][j][k + 2]);
					fprintf(fp, ")\\n");
				}
			}
			
			// for all valid entries in basicBlock of node 'i', print the basicBlock contents
			for (j = 1; j <= basicBlock[i][0]; j++)	{
				int temp = basicBlock[i][j];
				if (temp < 0)	{
					// temp <= -999 indicates the renamed value
					if (temp <= -999)	{
						// convert temp to equivalent positive value before printing
						if (temp < -999)
							fprintf(fp, "(%d) ", -(temp + 1000));
					}
					else	{
						// temp = -10 is to print a newline character
						if (temp == -10)
							fprintf(fp, "\\n");
						// for all other negative values of temp, print its
						// equivalent positive ASCII value
						else
							fprintf(fp, "%c", -temp);
					}
				}
				// temp >= 0 indicates variable
				else
					fprintf(fp, " t%d ", temp);
			}
			fprintf(fp, "\"]\n");
		}
	}
	// now connect the basicBlocks (nodes) using cfg (control flow graph) entries
	for (i = 0; i < NO_OF_NODES; i++)
		for (j = 0; j < NO_OF_NODES; j++)
			// check for 'i' to 'j' connection from cfg
			if (cfg[i][j] == 1)	// 1 indicates a valid edge
				fprintf(fp, "%d -> %d\n", i, j);
	fprintf(fp, "}\n");
}

/* ********************************* */
/* 
*/
int main (int argc, char *argv[])	{

	int i, j, k;	// indices
	int n;			// required number of IR generations
	FILE *fpGenIR, *fpSSA; // file pointers, in use with dotty

	fpGenIR = fopen("generatedIR.dot", "w");
	fpSSA = fopen("SSA.dot", "w");
	
	if (argc != 2)	{
		printf ("Usage: %s number_of_statements (per nesting).\n", argv[0]);
		exit (1);
	}
	n = atoi(argv[1]);
	
	// an overhead of initializing all arrays
	for (i = 0; i < NO_OF_NODES; i++)
		for (j = 0; j < NO_OF_VARIABLES_IN_A_BLOCK * EXPANDED_SIZE; j++)
			basicBlock[i][j] = -1;			// initializing basicBlock to -1
				
	for (i = 0; i < NO_OF_NODES; i++)
		for (j = 0; j < NO_OF_NODES; j++)
			cfg[i][j] = 0;					// initializing cfg to 0
			
	for (i = 0; i < NO_OF_NODES; i++)
		for (j = 0; j < NO_OF_NODES; j++)
			DF[i][j] = -1;					// initializing DF to -1
			
	for (i = 0; i < NO_OF_NODES; i++)
		for (j = 0; j < NO_OF_VARIABLES_IN_A_BLOCK; j++)
			aOrig[i][j] = -1;				// initializing aOrig to -1
	
	for (i = 0; i < NO_OF_NODES; i++)
		for (j = 0; j < NO_OF_VARIABLES; j++)
			aPHI[i][j] = -1;				// initializing aPHI to -1
			
	for (i = 0; i < NO_OF_NODES; i++)	{
		for (j = 0; j < NO_OF_VARIABLES; j++)	{
			aPHIstatements[i][j][0] = 0;
			for (k = 1; k < NO_OF_NODES * 2; k++)
				aPHIstatements[i][j][k] = -999;		// initializing aPHIstatements to -999 and aPHIstatements[i][j][0] to -1
		}
	}
	
	// to generate random IR with 'n' generation and current depth 0
	// call to genir.c:genir_random()
	genir_random (0, n, 0);

	// to manually enter required cfg or basicBlock or aOrig	
/*	printf("\nEnter cfg... How many nodes: ");
	scanf("%d", &i);
	printf("Enter now:\n");
	for (i = 0; i < i; i++)
		for (j = 0; j < i; j++)
			scanf("%d", &cfg[i][j]);
	
/*	printf("You entered the cfg:\n");
	for (i = 0; i < NO_OF_NODES; i++)
	{
		printf("\n");
		for (j = 0; j < NO_OF_NODES; j++)
			printf("%d\t", cfg[i][j]);
	}*/
	
/*	printf("\nEnter basic blocks:\n");
	for (i = 0; i < NO_OF_NODES; i++)
		for (j = 0; j < NO_OF_VARIABLES_IN_A_BLOCK * 4; j++)
			scanf("%d", &basicBlock[i][j]);
			
	printf("\nEnter aOrig:\n");
	for (i = 0; i < NO_OF_NODES; i++)
		for (j = 0; j < NO_OF_VARIABLES_IN_A_BLOCK; j++)
			scanf("%d", &aOrig[i][j]);*/
			
/*	printf("\naOrig:\n");
	for (i = 0; i < NO_OF_NODES; i++)	{
		for (j = 0; j < NO_OF_VARIABLES_IN_A_BLOCK; j++)
			printf("%d\t", aOrig[i][j]);
		printf("\n");
	}*/
			
//	dispBasicBlock();
	
	// display the generated IR in dotty
	dispInDotty(fpGenIR, "generatedIR");
	
	// to get the idominators of each node (getting the dominator tree)
	// call	to mainDominator
	mainDominator(NO_OF_NODES, cfg, idominator);
//	dispIDominator();

	
	// to compute Dominance Frontiers (DFn's) of each node
	// call to dfn.c:computeDFn()
	computeDFn(0);		// 0 indicates root node
//	dispDFn();
	
	// to place phi functions - SSA intermediate form
	// call to phi.c:placePHIfunctions()
	placePHIfunctions();
//	dispAPhi();	


	// to rename variables - SSA FORM
	// call to rename.c:initializeRenameStacks()
	initializeRenameStacks();
	// call to rename.c:renameVariables()
	renameVariables(0);		// 0 indicates root node

	// display the PHI placed IR in dotty
	dispInDotty(fpSSA, "SSA");
	
	return 0;
}

