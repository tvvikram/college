#include <stdio.h>
#include <stdlib.h>
#include "dominator.header.h"
#include "header.h"
#include "genir.h"
/* The lengauer-Tarjan Algorithm, as described in the book by Andrew W Appel (compiler implementation */
int N = 0;	// index
// buckets to handle vertices
int buckets[MAXIMUM_VERTICES][MAXIMUM_VERTICES] = {};

// to perform dfs visit
static void dfs (int cfg[][MAXIMUM_VERTICES], int no_of_vertices, int p, int n) {
		
	int i;	// index
	int w;	// successor of 'n'
	if (dfnum[n] == 0)	{
		dfnum[n] = N;
		vertex[N] = n;
		parent[n] = p;
		N++;
		// for each successor 'w' of 'n'
		for (i = 0; i < no_of_vertices; i++)	{
			w = cfg[n][i];
			if (w == 1)
				dfs(cfg, no_of_vertices, n, i);
		}
	}
}

void link(int p, int n)	{

	ancestor[n] = p;
}	

// naive version
int ancestorWithLowestSemidominator (int v) {

	int u = v;	// initially u = v
	// check for node 'u' having the lowest semidominator value
	// this needs 'v' to move up until a semidom is met
	while (ancestor[v] != NONE)	{
		if (dfnum[semi[v]] < dfnum[semi[u]])
			u = v;
		v = ancestor[v];
	}
	return u;
}


/* node 0 is assumed to be the root */
/* for any other root, modify the call to dfs, 
and the last statement to set dominator of the root */

int build_dominator_tree (int cfg[][MAXIMUM_VERTICES], int idominator[], int no_of_vertices) {
	
	int i, j, k;	// indices
	int n;			// node
	int p;			// parent
	int v;			// predecessor of 'n'
	int s, tempS;	// semidominator candidates
	int y;			// lowest semidominator ancestor of 's'
	
	// initializations
	for (i = 0; i < no_of_vertices; i++)	{
		idominator[i] = NONE;
		dfnum[i] = 0;
		semi[i] = NONE;
		ancestor[i] = NONE;
		samedom[i] = NONE;
	}
	// call to dfs()
	dfs(cfg, no_of_vertices, NONE, 0);
	
	// iterate from the bottom-rightmost node till root node, skipping root node
	for (i = N - 1; i > 0; i--)	{
		n = vertex[i];
		p = parent[n];
		s = p;
		// to calculate the semidominator of 'n' using the Semidominator Theorem
		// for each predecessor 'v' of 'n'
		for (j = 0; j < no_of_vertices; j++)	{
			if (cfg[j][n] == 1)	{
				v = j;
				// 'v' is proper ancestor of 'n' in the spanning tree, if
				// dfnum(v) > dfnum(n). 'v' becomes candidate for semi(n).
				// True for the 1st predecessor ('v') of 'n'.
				if (dfnum[v] <= dfnum[n])
					tempS = v;
				// if 'u' is an ancestor of 'v' or if 'u = v' and dfnum(v) > dfnum(n).
				// semi(u) becomes candidate for semi(n).
				else
					tempS = semi[ancestorWithLowestSemidominator(v)];
				if (dfnum[tempS] < dfnum[s])
					s = tempS;
			}
		}
		semi[n] = s;
		// update the bucket 's' with 'n'
		for (k = 0; buckets[s][k] != 0; k++);
		buckets[s][k] = n;
		link(p, n);
		// to calculate the semidominator of 'v'
		// idom(n) = semi(n), if semi(y) = semi(n)
		for (k = 0; buckets[p][k] != 0; k++)	{
			v = buckets[p][k];
			y = ancestorWithLowestSemidominator(v);
			if (semi[y] == semi[v])
				idominator[v] = p;
			else
				samedom[v] = y;
		}
		for (k = 0; buckets[p][k] != 0; k++)
			buckets[p][k] = 0;
	}
	// for all vertices, calculating,
	// idom(n) = idom(y), if semi(y) != semi(n)
	for (i = 1; i < N; i++)	{
		n = vertex[i];
		if (samedom[n] != NONE)
			idominator[n] = idominator[samedom[n]];
	}
	return 0; 
}

