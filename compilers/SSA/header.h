#include<stdio.h>

#define NO_OF_VARIABLES_IN_A_BLOCK 25
#define NO_OF_VARIABLES 25	// total number of variables in IR generation
#define NO_OF_NODES 100
#define EXPANDED_SIZE 8


// all required arrays are declared as global and initialized in main.c:main()

// these declarations are with respect to determining dominator tree only
int dfnum[NO_OF_NODES];
int vertex[NO_OF_NODES];
int parent[NO_OF_NODES];
int ancestor[NO_OF_NODES];
int semi[NO_OF_NODES];
int samedom[NO_OF_NODES];

// these declarations are with respect to generation of the 3 address code
// and handling them.

// basicBlock stores the BasicBlock contents, with a header total count;all variables
// are positive values, followed by a less than -999 value, which can be decoded to
// obtain the renamed value. All other symbols like +, -, *, /, <, >, = and other symbols
// and alphabets are stored as negative ASCII values. These can be decoded properly
// while printing using dotty. Initialized to -1 by default.
int basicBlock[NO_OF_NODES][NO_OF_VARIABLES_IN_A_BLOCK * EXPANDED_SIZE];

// cfg stores the various interconnections between the nodes or the blocks.
// if there is an edge between two nodes '1' is marked else by default its '0'.
// Initialized to '0' by default.
int cfg[NO_OF_NODES][NO_OF_NODES];
// basicBlock alongwith cfg determine the Control Flow Graph.

// idominator array gives the idominator for each of the nodes in the graph.
// Initialized to -1 by default.
int idominator[NO_OF_NODES];

// DF gives the dominance frontiers of the nodes. Initialized to -1 by default.
int DF[NO_OF_NODES][NO_OF_NODES];

// aOrig contains the latest defined variables. Initialized to -1 by default.
int aOrig[NO_OF_NODES][NO_OF_VARIABLES_IN_A_BLOCK];

// aPHI contains the nodes that must have phi functions for the variables at that node.
// Initialized to -1 by default.
int aPHI[NO_OF_NODES][NO_OF_VARIABLES];

// aPHIstatements contains the phi statements for the variables at a particular node.
// First memory space for each variable of a particular node contains the header count, 
// initialized to 0 by default. The next memory spaces contain the variable's renamed
// values which are less than -999, initialized to -999 by default, and can be decoded
// accordingly while printing using dotty.
int aPHIstatements[NO_OF_NODES][NO_OF_VARIABLES][NO_OF_NODES * 2];
