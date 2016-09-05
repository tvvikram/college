#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genir.h"
#include "header.h"
#include <unistd.h>


/* We generate the following kinds of code.

1. x = y 'op' z
2. x = 'op' y		('op' is a unary operator)
3. x = y
4. goto L - As of now, used only inside if x 'relop' y (for if else)
5. if x 'relop' y goto L

These still need to be added.
6. x = y[i] and x[i] = y
7. x = &y, x = *y and *x = y

NOTE: as of now, only integer datatypes are used. 
*/

static int label_count = 1;

int ifStack[100] = {}, ifIndex = 0, loopStack[100] = {}, loopIndex = 0, maxIfIndex, maxLoopIndex;

// to display the basicBlock contents
int dispBasicBlock()	{

	int m, n;	// indices
	printf("\nBasicBlock:\n");
	for (m = 0; m < NO_OF_NODES; m++)
	{
		for (n = 0; n < NO_OF_VARIABLES_IN_A_BLOCK * EXPANDED_SIZE; n++)
			printf("%d\t", basicBlock[m][n]);
		printf("\n");
	}
	return 0;
}

// inserts the variable to the basic block
int insertToBasicBlock(int blockNo, int variable)	{

	/*
		basicBlock initially contains every element as -1,
		all symbols are given ASCII equivalent negative values:
	*/
	int i;	// index
	// move to the last available space in the basicBlock[blockNo]
	for (i = 1; i < NO_OF_VARIABLES_IN_A_BLOCK * EXPANDED_SIZE 
				&& basicBlock[blockNo][i] != -1; i++);
	// if there is not enough space to insert a new variable,
	// quit the program
	if (i >= NO_OF_VARIABLES_IN_A_BLOCK * EXPANDED_SIZE)	{
		printf("genir.c:Too many generations... ");
		printf("Cannot be accomodated within basicBlock at node %d.\n", blockNo);
		printf("genir.c:I am quitting.\n");
		exit(1);
	}
	// insert variable at the end of basicBlock[blockNo]
	basicBlock[blockNo][i] = variable;
	// simultaneously reserve space for renaming variables
	// using an arbitrary value
	if (variable >= 0)
		basicBlock[blockNo][++i] = -999;	
	// update the header count that keeps track of last position of insertion,
	// for ease of printing with dotty
	basicBlock[blockNo][0] = i;				
}

// inserts the defined variable to aOrig
// position of insertion is returned
int insertToAOrig(int blockNo, int variable)	{

	int i, j;	// indices
	// variable name cannot be less than 0
	if (variable < 0)	{
		printf("\nIllegal variable name (%d).\n", variable);
		return -1;			// if so, its a false definition
	}
	// insert to aOrig[blockNo], contains the latest defined variables of that block
	for (i = 0; aOrig[blockNo][i] != -1; i++)	{
		// if the defined variable is already in the aOrig[blockNo]
		if (aOrig[blockNo][i] == variable)	{
			// move the other variables and insert variable at last
			for (j = i; aOrig[blockNo][j] != -1; j++)	
				aOrig[blockNo][j] = aOrig[blockNo][j + 1];
			aOrig[blockNo][--j] = variable;
			return j;
		}
	}
	// if variable is not defined, insert the variable at last
	aOrig[blockNo][i] = variable;
	return i;
}

// to generate IR with required generations and required depth
void genir_random (int current_depth, int required_generations, int blockNo)	{
	
	int generations, random_value;
	srand ((int)time (NULL));
	
	for (generations = 0; generations < required_generations; generations++)	{
		
		random_value = random () % 12;
		switch (random_value) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				{
				/* x = y 'op' z */
				int x, y, z; char op;
				x = random() % NO_OF_VARIABLES;
				insertToBasicBlock(blockNo, x);
				insertToBasicBlock(blockNo, -61);
				/* Decide if y is a constant of a variable.
				1/3rd probability for a constant */
				if ((random() % 3) == 0)
					y = -1;
				else
					y = random() % NO_OF_VARIABLES;
				/* similarly for z */
				if ((random() % 3) == 0)
					z = -1;
				else
					z = random() % NO_OF_VARIABLES;
				/* Now decide on the operator */
				switch (random() % 4) {
					case 0:
						op = '+';
						break;
					case 1:
						op = '-';
						break;
					case 2:
						op = '*';
						break;
					case 3:
						op = '/';
						break;
				}
				if (y == -1 && z == -1)
				printf ("\tt%d = %d %c %d\n", x, (int)random(), op, (int)random());
				else if (y == -1 && z >= 0)
				printf ("\tt%d = %d %c t%d\n", x, (int)random(), op, z);
				else if (y >= 0 && z == -1)
				printf ("\tt%d = t%d %c %d\n", x, y, op, (int)random());
				else if (y >= 0 && z >= 0)
				printf ("\tt%d = t%d %c t%d\n", x, y, op, z);
				insertToAOrig(blockNo, x);					// insert the definition 'x' to aOrig
				if (y < 0)
					insertToBasicBlock(blockNo, -67);		// insert 'C' to indicate constant
				else
					insertToBasicBlock(blockNo, y);			// else insert 'y'
				insertToBasicBlock(blockNo, -op);			// insert the operand
				if (z < 0)
					insertToBasicBlock(blockNo, -67);		// for 'z'
				else
					insertToBasicBlock(blockNo, z);
				insertToBasicBlock(blockNo, -10);			// insert end of line
				break;
			}
			case 5: {
				/* x = -y (currently only unary minus) */
				int x, y;
				x = random() % NO_OF_VARIABLES;
				insertToBasicBlock(blockNo, x);
				insertToBasicBlock(blockNo, -61);
				/* decide if y is a variable or a constant */
				y = random() % NO_OF_VARIABLES;
				if (random() % 3 == 0) // one of three chances is a constant
					y = -1;
				if (y == -1)
					printf ("\tt%d = -%d\n", x, (int)random());
				else
					printf ("\tt%d = -t%d\n", x, y);
				insertToAOrig(blockNo, x);					// insert the definition 'x' to aOrig
				insertToBasicBlock(blockNo, -45);			// insert the operand
				if (y < 0)
					insertToBasicBlock(blockNo, -67);
				else
					insertToBasicBlock(blockNo, y);
				insertToBasicBlock(blockNo, -10);	
				break;
			}
			case 6:
			case 7: {
				/* Copy statements */
				int x, y;
				x = random() % NO_OF_VARIABLES;
				insertToBasicBlock(blockNo, x);
				insertToBasicBlock(blockNo, -61);
				y = random() % NO_OF_VARIABLES;
				if ((random() % 4) == 0) // one of four chances is constant
					y = -1;
				if (y == -1)
					printf ("\tt%d = %d\n", x, (int)random());
				else
					printf ("\tt%d = t%d\n", x, y);
				insertToAOrig(blockNo, x);					// insert the definition 'x' to aOrig
				if (y < 0)
					insertToBasicBlock(blockNo, -67);
				else
					insertToBasicBlock(blockNo, y);
				insertToBasicBlock(blockNo, -10);
				break;
			}
			case 8: 
			case 9:
				{
				/* generate if and if else statements */
				int ifbegin, ifend, x, y, nest_size, elsebegin;
				char relop[3];
				
				if (current_depth >= MAX_DEPTH || label_count >= NO_OF_NODES) break;
				/* we shouldn't exceed the maximum nesting depth */
				
				ifbegin = label_count++;
				ifend = label_count++;
				elsebegin = label_count++;
				
				x = random() % NO_OF_VARIABLES;
				y = random() % NO_OF_VARIABLES;
				
				if (random() % 3 == 0) // one of three chances, x is a constant.
					x = -1;
				if (random() % 3 == 0) // one of three chances, y is a constant.
					y = -1;
				/* The case where both x and y are constants, evaluates
				to a constant expression. This might help in dead code elimination
				and other simplification. */

				switch (random() % 6) {
					case 0:
						strcpy (relop, "==");
						break;
					case 1:
						strcpy (relop, "!=");
						break;
					case 2:
						strcpy (relop, "<");
						break;
					case 3:
						strcpy (relop, "<=");
						break;
					case 4:
						strcpy (relop, ">");
						break;
					case 5:
						strcpy (relop, ">=");
						break;
				}
				if (x == -1 && y == -1)
					printf ("\tif %d %s %d goto ifbegin%d\n", 
						 (int)random(), relop, (int)random(), ifbegin);
				else if (x == -1 &&  y >= 0)
					printf ("\tif %d %s t%d goto ifbegin%d\n",
						 (int)random(), relop, y, ifbegin);
				else if (x >= 0 && y == -1)
					printf ("\tif t%d %s %d goto ifbegin%d\n",
						 x, relop, (int)random(), ifbegin);
				else if (x >= 0 && y >= 0)
					printf ("\tif t%d %s t%d goto ifbegin%d\n",
						 x, relop, y, ifbegin);
						 
				insertToBasicBlock(blockNo, -105);				// ASCII equivalents of 'if'
				insertToBasicBlock(blockNo, -102);
				if (x < 0)
					insertToBasicBlock(blockNo, -67);
				else
					insertToBasicBlock(blockNo, x);
				insertToBasicBlock(blockNo, -relop[0]);
				if (relop[1] != '\0')
					insertToBasicBlock(blockNo, -relop[1]);
				if (y < 0)
					insertToBasicBlock(blockNo, -67);
				else
					insertToBasicBlock(blockNo, y);
				insertToBasicBlock(blockNo, -10);

				/* decide whether if or if else */

				if (random() % 2 == 0) { /* equal chances for both */
					/*
						if x 'relop' y goto ifbegin
						goto ifend
					ifbegin:
						....
						....
					ifend:
						...
					*/

					if (ifIndex == 0 || ifIndex == 1)
						maxIfIndex = 0;

					ifStack[++ifIndex] = ifbegin;	// using stacks to save
					ifStack[++ifIndex] = blockNo;	// the block numbers
					
					printf ("\tgoto ifend%d\n", ifend);
					printf ("ifbegin%d:\n", ifbegin);
					// insert 'Y', to indicate the start of 'ifbegin',
					// avoids confusion during display
					insertToBasicBlock(ifbegin, -89);
					nest_size = (random()%(NEST_SIZE_MAX-NEST_SIZE_MIN))+NEST_SIZE_MIN;
					// connect to 'ifbegin' block from existing 'if' block
					cfg[blockNo][ifbegin] = 1;
					if (ifIndex > maxIfIndex)
						maxIfIndex = ifIndex;
					genir_random (current_depth+1, nest_size, ifbegin);
					printf ("ifend%d:\n", ifend);

					cfg[ifStack[ifIndex--]][ifend] = 1;	// connect the blocks by
					cfg[ifStack[ifIndex--]][ifend] = 1;	// referring to stacks also
					if (maxIfIndex != ifIndex + 2)
						ifIndex--;
					ifStack[++ifIndex] = ifend;
		
					blockNo = ifend;	// finally assign blockNo to the latest defined block
				}
				else {
					/*
						if x 'relop' y goto ifbegin
						goto elsebegin
					ifbegin:
						....
						....
						goto ifend
					elsebegin:
						....
						....
					ifend:
						...
					*/

					if (ifIndex == 0 || ifIndex == 1)
						maxIfIndex = 0;
					
					ifStack[++ifIndex] = ifbegin;	// using stacks to save
					ifStack[++ifIndex] = elsebegin;	// the block numbers
					
					printf ("\tgoto elsebegin%d\n", elsebegin);
					printf ("ifbegin%d:\n", ifbegin);
					// insert 'Y', to indicate the start of 'ifbegin',
					// avoids confusion during display
					insertToBasicBlock(ifbegin, -89);		
					nest_size = (random()%(NEST_SIZE_MAX-NEST_SIZE_MIN))+NEST_SIZE_MIN;
					// connect to 'ifbegin' block from existing 'if' block
					cfg[blockNo][ifbegin] = 1;
					if (ifIndex > maxIfIndex)
						maxIfIndex = ifIndex;
					genir_random (current_depth+1, nest_size, ifbegin);
					printf ("\tgoto ifend%d\n", ifend);
					printf ("elsebegin%d:\n", elsebegin);
					// connect to 'elsebegin' block from existing 'if' block
					cfg[blockNo][elsebegin] = 1;					
					nest_size = (random()%(NEST_SIZE_MAX-NEST_SIZE_MIN))+NEST_SIZE_MIN;
					if (ifIndex > maxIfIndex)
						maxIfIndex = ifIndex;
					genir_random (current_depth+1, nest_size, elsebegin);
					printf ("ifend%d:\n", ifend);

					cfg[ifStack[ifIndex--]][ifend] = 1;	// connect the blocks by
					cfg[ifStack[ifIndex--]][ifend] = 1;	// referring to stacks also
					if (maxIfIndex != ifIndex + 2)
					{
						ifIndex--;	// once along if path
						ifIndex--;	// once along else path
					}
					ifStack[++ifIndex] = ifend;
		
					blockNo = ifend;	// finally assign blockNo to the latest defined block
				}
				break;
			}
			case 10:
			case 11: {
				/* Lets have a while loop now */
				int loopbegin, loopend, loopbody, x, y;
				char relop[3];
				int nest_size;
				
				if (current_depth >= MAX_DEPTH || label_count >= NO_OF_NODES) break;
				/* we shouldn't exceed the maximum nesting depth */
				
				loopbegin = label_count++;
				loopbody = label_count++;
				loopend = label_count++;
				
				nest_size = (random()%(NEST_SIZE_MAX-NEST_SIZE_MIN))+NEST_SIZE_MIN;
				
				x = random() % NO_OF_VARIABLES;
				y = random() % NO_OF_VARIABLES;
				
				if (random() % 3 == 0) // one of three chances, x is a constant.
					x = -1;
				if (random() % 3 == 0) // one of three chances, y is a constant.
					y = -1;
				/* The case where both x and y are constants, evaluates
				to a constant expression. This might help in dead code elimination
				and other simplification. */

				switch (random() % 6) {
					case 0:
						strcpy (relop, "==");
						break;
					case 1:
						strcpy (relop, "!=");
						break;
					case 2:
						strcpy (relop, "<");
						break;
					case 3:
						strcpy (relop, "<=");
						break;
					case 4:
						strcpy (relop, ">");
						break;
					case 5:
						strcpy (relop, ">=");
						break;
				}
				
				/* 
				loopbegin: 
					if x 'relop' y goto loopbody
					goto loopend 
				loopbody:
					.....
					.....
					goto loopbegin
				loopend:
				*/
				printf ("loopbegin%d:\n", loopbegin);
				if (x == -1 && y == -1)
					printf ("\tif %d %s %d goto loopbody%d\n", 
						 (int)random(), relop, (int)random(), loopbody);
				else if (x == -1 &&  y >= 0)
					printf ("\tif %d %s t%d goto loopbody%d\n",
						(int)random(), relop, y, loopbody);
				else if (x >= 0 && y == -1)
					printf ("\tif t%d %s %d goto loopbody%d\n",
						x, relop, (int)random(), loopbody);
				else if (x >= 0 && y >= 0)
					printf ("\tif t%d %s t%d goto loopbody%d\n",
						x, relop, y, loopbody);

				insertToBasicBlock(loopbegin, -105);		// to print 'if'
				insertToBasicBlock(loopbegin, -102);
				if (x < 0)
					insertToBasicBlock(loopbegin, -67);
				else
					insertToBasicBlock(loopbegin, x);
				insertToBasicBlock(loopbegin, -relop[0]);
				if (relop[1] != '\0')
					insertToBasicBlock(loopbegin, -relop[1]);
				if (y < 0)
					insertToBasicBlock(loopbegin, -67);
				else
					insertToBasicBlock(loopbegin, y);
				insertToBasicBlock(loopbegin, -10);
				
				printf ("\tgoto loopend%d\n", loopend);
				printf ("loopbody%d:\n", loopbody);
				// connect to 'loopbegin' block from existing 'conditional' block
				cfg[blockNo][loopbegin] = 1;
				// connect the 'loopbegin' block to the 'loopbody'
				cfg[loopbegin][loopbody] = 1;
				// connect the 'loopbegin' block to the 'loopend'
				cfg[loopbegin][loopend] = 1;
				// maintaining a stack to save the loopbegin numbers
				if (loopIndex == 0)
					maxLoopIndex = 0;
				loopStack[++loopIndex] = loopend;
				loopStack[++loopIndex] = loopbegin;
				if (loopIndex > maxLoopIndex)
					maxLoopIndex = loopIndex;
				genir_random (current_depth+1, nest_size, loopbody);
				printf ("\tgoto loopbegin%d\n", loopbegin);
				
				if (maxLoopIndex == loopIndex)
				{
					cfg[loopbody][loopbegin] = 1;
					loopIndex--;
				}
				else
				{
					if (loopIndex != 1)
					{
						int temp1, temp2;
						temp1 = loopIndex--;
						temp2 = loopIndex--;
						cfg[loopStack[temp1]][loopStack[temp2]] = 1;
					}
				}
				printf ("loopend%d:\n", loopend);
				blockNo = loopend;	// assign blockNo with loopend, the latest defined block
				break;
			}
		}
	}
}

