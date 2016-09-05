#include<stdio.h>
#include<stdlib.h>
#include "header.h"
#define STACK_SIZE 20

// separate count for each variable
int count[NO_OF_VARIABLES] = {};
// separate stack for each variable
int stack[NO_OF_VARIABLES][STACK_SIZE];

// to initialise stacks
void initializeRenameStacks()	{

	int i, j;	// indices
	for (i = 0; i < NO_OF_VARIABLES; i++)
	{	stack[i][0] = 0;		// push 0 to each stack
		for (j = 1; j < STACK_SIZE; j++)
			stack[i][j] = -1;	// initialize to -1
	}
}

// stack: pushing onto the variable stack
void push(int variable, int value)	{

	int i;	// index
	for (i = 0; stack[variable][i] != -1; i++);
	stack[variable][i] = value;
}

// stack: return top of the variable stack
int top(int variable)	{

	int i;	// index
	for (i = 0; stack[variable][i] != -1; i++);
	return stack[variable][--i];
}

// stack: pop the top of the variable stack
int pop(int variable)	{

	int i;	// index
	int saveTop;	// to save the top of stack
	for (i = 0; stack[variable][i] != -1; i++);
	saveTop = stack[variable][--i];
	stack[variable][i] = -1;	// erase the previously stored top value
	return saveTop;
}

// to rename variables for the phi placed IR - SSA FORM
void renameVariables(int n)	{

	int variable[3], varIndex = 0;  // stores 3 addresses of variables from a statement, like if,
									// a = b + c; variable[0] has "a's" address, variable[1]
									// has "b's" address and variable[2] has "c's" address
	int i, j;	// index
	int Y, p, var, X;
	int temp, renameVar, ifFlag = 1;
	
	variable[0] = variable[1] = variable[2] = -1;	// initialization

	// for each variable of block 'n'; for all phi entries, rename the definition of the variable
	for (i = 0; i < NO_OF_VARIABLES; i++)	{
		if (aPHIstatements[n][i][0] > 0)	{
			renameVar = ++count[i];
			push(i, renameVar);
			// 1st position in aPHIstatements is for renamed definition of variable 'i' of block 'n'
			aPHIstatements[n][i][1] = renameVar;	// renaming "variable" as "variable(renameVar)"
		}
	}
	
	// for each element of basicBlock entry
	for (i = 1; i <= basicBlock[n][0]; i++)	{
		temp = basicBlock[n][i];
		// temp >= 0 indicates it as a variable
		if (temp >= 0)
			variable[varIndex++] = i;
		// -105 is negative ASCII code for 'i', is definitely known to be followed by 'f',
		// forming 'if' statement
		if (temp == -105)
			ifFlag = 0;
		// temp = -10 indicates newline in basicBlock, marks the end of a statement
		if (temp == -10)	{
			if (varIndex > 3)
				printf("\nrename.c:Not in 3 address format.\n");
			// if there is an 'if' statement
			if (!ifFlag)	{
				// make the definition parts as the use parts in variable array
				variable[2] = variable[1];
				variable[1] = variable[0];
				variable[0] = -1;
			}
			// if there is a first use of some variable found
			if (variable[1] != -1)	{
				// rename it as variable(renameVar)
				// get top of stack[firstUseVariable] to renameVar
				renameVar = top(basicBlock[n][variable[1]]);
				// store the renamed value as a value above -999
				basicBlock[n][variable[1] + 1] = -(renameVar + 1000);
			}
			// if there is a second use of some variable found
			if (variable[2] != -1)	{
				// rename it as variable(renameVar)
				// get top of stack[secondUseVariable] to renameVar
				renameVar = top(basicBlock[n][variable[2]]);
				// store the renamed value as a value above -999				
				basicBlock[n][variable[2] + 1] = -(renameVar + 1000);
			}
			// if there is no 'if' statement, rename the definition
			if (ifFlag)	{
				// increment the count of the 'definitionVariable'
				renameVar = ++count[basicBlock[n][variable[0]]];
				// push to top of stack[definitionVariable]
				push(basicBlock[n][variable[0]], renameVar);
				// rename as variable(renameVar)
				basicBlock[n][variable[0] + 1] = -(renameVar + 1000);
			}
			// reset the variable array and its index
			variable[0] = variable[1] = variable[2] = -1;
			varIndex = 0;
		}
	}

	// 	for each successor 'Y' of block 'n'
	for (Y = 0; Y < NO_OF_NODES; Y++)	{
		j = 0;
		if (cfg[n][Y] == 1)	{		// for each successor Y of n
			for (p = 0; p < NO_OF_NODES; p++)	{
				// find the predecessors of 'Y'
				if (cfg[p][Y] == 1)
					j++;				// 'j' contains the jth predecessor of 'Y' as 'n'
				if (p == n)
					p = NO_OF_NODES;	// break the loop
			}
			// for each variable 'var' of block 'Y'
			for (var = 0; var < NO_OF_VARIABLES && aPHIstatements[Y][var][0] != -1; var++)	{
				// if there exists a jth operand of phi function
				if (aPHIstatements[Y][var][0] >= j)	{
					// then rename it as variable(renameVar)
					renameVar = top(var);	// get the top of stack[variable]
					aPHIstatements[Y][var][j + 1] = renameVar;
				}
			}
		}// end of 'if'
	}// end of 'for'

	// for each child 'X' of 'n'
	for (X = 0; X < NO_OF_NODES; X++)
		if (idominator[X] == n)
			// recursively call for renaming variables
			renameVariables(X);
	
	// for each element of basicBlock entry
	for (i = 1; i <= basicBlock[n][0]; i++)	{
		temp = basicBlock[n][i];
		// get the variables in the variable
		if (temp >= 0)
			variable[varIndex++] = i;
		// temp = -10 indicates newline, marks the end of the statement
		if (temp == -10)	{
			if (varIndex > 3)
				printf("\nrename.c:Not in 3 address format\n");
			// for the definition of a variable at variable[0],
			// pop from stack[variable]
			pop(basicBlock[n][variable[0]]);
			// reset both the variable array and its index
			variable[0] = variable[1] = variable[2] = -1;
			varIndex = 0;
		}
	}			
}// end of renameVariables()

