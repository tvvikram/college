#ifndef HEADER_H
#define HEADER_H

#define MAXIMUM_VERTICES 1000
#define NONE -1
#define true 1
#define false 0
#define VISITED 0
#define UNVISITED -1

/*struct Node {
	int number;
	struct Node *next;
};*/

/* -------------------------------------------------------------------------- */
/* callable functions in dominator.c */

/* node 0 is assumed to be the root */
int build_dominator_tree (int cfg[][MAXIMUM_VERTICES], int idominator[], int no_of_vertices);

/* 
IMPORTANT: 1. This function is only for reducible graphs. 
2. The cfg[][] matrix is changed. Make a copy before calling.
*/
void build_dominator_tree_reducible_cfg (int cfg[][MAXIMUM_VERTICES], int idominator[], int no_of_vertices);

/* --------------------------------------------------------------------------- */

/* callable functions in  gencfg.c */

/* Also calculates the dominator tree for the CFG generated
   The function fails (may crash) if no_of_vertices is lesser than 2 */
void generate_cfg (int cfg[][MAXIMUM_VERTICES], int no_of_vertices, int idominator[]);

/* --------------------------------------------------------------------------- */
#endif
