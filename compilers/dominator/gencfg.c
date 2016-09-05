#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "header.h"

static void dfs (int cfg[][MAXIMUM_VERTICES], int wrt, int no_of_vertices);
static void find_dominator (int cfg[][MAXIMUM_VERTICES], int wrt, int no_of_vertices, int idominator[]);

static int dfs_number[MAXIMUM_VERTICES] = {}, dfs_counter = 0; 
char leaves[MAXIMUM_VERTICES], dfs_tree[MAXIMUM_VERTICES][MAXIMUM_VERTICES] = {};

/* We can actually reuse the arrays in the above declarations. We will be using dominator_set and
final_dominator_set only after indegree_forward_edges and leaves are no more needed */


void generate_cfg (int cfg[][MAXIMUM_VERTICES], int no_of_vertices, int idominator[]) {
/* Also calculates the dominator tree for the CFG generated */
/* The function fails (may crash) if no_of_vertices is lesser than 2 */
	
	int i, j, u, v, count, decision;
	int successors;
			
	/* A few initialisations */
	for (i = 0; i < no_of_vertices; i++)
		for (j = 0; j < no_of_vertices; j++)
			cfg[i][j] = 0;
	
	for (i = 0; i < no_of_vertices; i++)
		leaves[i] = UNVISITED;
	for (i = 0; i < no_of_vertices; i++)
		idominator[i] = NONE;
	
	count = (int) time (NULL);
	fprintf (stderr, "Seed value: %d\n", count);
	/* seed the random() funtion */
	srandom (count);
	
	/* add "no_of_vertices-2" vertices. root already exists
	   and one vertex will be added as the terminal vertex */
	
	for (count = 1; count < no_of_vertices-1; count++) {
		
		/* choose a vertex among the present vertices randomly */
		/* we either add a loop or a new successor to this vertex */
		u = random() % count;
		
		/* now decide whether to add a loop or just a new successor */
		decision = random() % 2;
		/* I just chose this ratio. Needs further thinking */
		/* 1 out of 2 hits will be for a loop. */
		if (decision == 0) {
			cfg[u][u] = 0;
			cfg[count][u] = 1;
			/* add an edge from the new vertex to this vertex */
			/* AFTER REDUCTION, this will form the self loop */
			/* Note that there will be no self-loops in the graph generated.
			   If this is needed, We need to add edges of the form cfg[x][x] */
			/* We do not add to indegree_forward_edges[] here (obvious) */
		}
		
		/* divide the successors of 'u' into three sections */
		/* common successors are those common to the child and the parent, before reduction */
		/* acquired successors are those acquired during reduction */
		/* original successors are successors only to the parent */

		for (j = 0; j < count; j++) {
			if (cfg[u][j] == 1) {
				decision = random() % 3;
				if (decision == 0) { /* common successors */
					cfg[count][j] = 1;
					/* This is the only case where edges are actually 
					   added to the CFG. Therefore, controlling the 
					   probability of occurance of this case, we can
					   control the density of the generated CFG */
				}
				else if (decision == 1) {  
					/* acquired successors. In this case, we will not be
					able to predict how and when the idominator of 'j' 
					might change. So we put it as unknown (NONE). */
					idominator[j] = NONE;
					cfg[count][j] = 1;
					cfg[u][j] = 0;
				}
				/* else, original successors. No operation */
			}
		}
		/* add the new successor to 'u' */
		cfg[u][count] = 1;
		idominator[count] = u;
		/* Since in t1-t2 reduction, only nodes with unique predecessors are reduced,
		this unique predecessor of a node is also its dominator */
	}

	/* now add a terminating vertex - perform a depth first search of the first n-1 vertices.
	Leaves of the DFS spanning tree will be led into terminating vertex. Count now points to 
	the final vertex. */
	dfs (cfg, 0, no_of_vertices-1);
	for (i = 0; i < no_of_vertices-1; i++)
		if (leaves[i] == true)
			cfg[i][count] = 1;
	
	/* Add an edge from the first leaf (not counting the terminating vertex) to the terminating vertex
	in the dfs_tree, to complete the dfs_tree */

	i = 0;
	while (true) {
		for (j = 0; j < no_of_vertices; j++)
			if (dfs_tree[i][j] == 1) {
				i = j;
				break;
			}
		if (j == no_of_vertices)
			break;
	}
	dfs_tree[i][count] = 1;

	/* We now have to determine idominator[] for those nodes which have a value of NONE.
	The algorithm for finding a dominator set is from Aho-Sethi-Ullman. For each
	predecessor of the terminating vertex, we find the dominator set and take an 
	intersection of all these. Of the nodes in this intersection, the one with the
	highest dfs_number is the immediate dominator.
	A Recursive function is used that finds idominator[] for a node, only if its not yet 
	known. If already known, it makes use of it. */

	/* NOTE we need to find the percentage of nodes for which idominator has already been
	calculated, to find the improvement over the basic algorithm given in the book. This
	can be done by keeping a count. */

	for (i = 1; i < no_of_vertices; i++)
		if (idominator[i] == NONE)
			find_dominator (cfg, i, no_of_vertices, idominator);
	return;
}

void find_dominator (int cfg[][MAXIMUM_VERTICES], int wrt, int no_of_vertices, int idominator[]) {
	
	char dominator_set[MAXIMUM_VERTICES], final_dominator_set[MAXIMUM_VERTICES];
	int i, j, u, v, test;
	
	/* the final_dominator_set is initialised to contain all the nodes. as we go on
	taking the intersections, the set converges */
	for (i = 0; i < no_of_vertices; i++)
		final_dominator_set[i] = 1;
	
	for (i = 0; i < no_of_vertices; i++)
		if (cfg[i][wrt] == 1) { /* for each predecessor 'i' of wrt */
			if (dfs_number[i] >= dfs_number[wrt]) {
				/* if dfs_number for 'i' is lesser than that for 'wrt', then the
				edge concerned is definitely not a looping edge. Otherwise, it may 
				be a loop edge or a cross edge. We need to determine which. If loop
				edge, then we need to ignore 'i' as a predecessor. if 'wrt' is a 
				spanning tree (dfs_tree) ancestor of 'i', then the edge is a loop edge.*/
				u = i;
				test = true;
				while (u != 0) {
					for (v = 0; v < no_of_vertices; v++)
						if (dfs_tree[v][u] == 1) {
							u = v;
							break;
						}
					if (u == wrt) {
						test = false;
						break;
					}
				}
				/* i == wrt implies a self loop. Adding this at the beginning would
				be more efficient. */
				if (test == false || i == wrt)
					continue;
			}
			
			for (j = 0; j < no_of_vertices-1; j++)
				dominator_set[j] = 0;
			u = i;
			while (u != 0) {
				if (idominator[u] == NONE)
					find_dominator (cfg, u, no_of_vertices, idominator);
				dominator_set[u] = 1;
				u = idominator[u];
			}
			dominator_set[0] = 1;
			/* now AND (intersection) dominator_set[] with final_dominator_set[] */
			for (j = 0; j < no_of_vertices; j++)
				final_dominator_set[j] = final_dominator_set[j] && dominator_set[j];
		}
		
	/*Now, among those vertices in the set, select the one with the highest dfs_number */
	v = 0; /* root is taken as the initial estimate */
	for (i = 0; i < no_of_vertices; i++)
		if (final_dominator_set[i] == 1) /* If node is in the set */
			if (dfs_number[i] > dfs_number[v])
				v = i;
	idominator[wrt] = v;
	
	return;
}
static void dfs (int cfg[][MAXIMUM_VERTICES], int wrt, int no_of_vertices) {

	int is_leaf = true, j;
	leaves[wrt] = VISITED;
	dfs_number[wrt] = dfs_counter++;
	
	for (j = 0; j < no_of_vertices; j++)
		if (cfg[wrt][j] == 1 && leaves[j] == UNVISITED) {
			dfs_tree[wrt][j] = 1;
			is_leaf = false;
			dfs (cfg, j, no_of_vertices);
		}
	
	if (is_leaf == true)
		leaves[wrt] = true;
}
