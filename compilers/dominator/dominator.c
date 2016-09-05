#include <stdio.h>
#include <stdlib.h>
#include "header.h"

/* The lengauer-Tarjan Algorithm, as described in the book by Andrew W Appel (compiler implementation */
int N = 0;
int dfnum[MAXIMUM_VERTICES] = {};
int vertex[MAXIMUM_VERTICES] = {};
int parent[MAXIMUM_VERTICES] = {};
int ancestor[MAXIMUM_VERTICES] = {};
int semi[MAXIMUM_VERTICES] = {};
int samedom[MAXIMUM_VERTICES] = {};
int buckets[MAXIMUM_VERTICES][MAXIMUM_VERTICES] = {};

static void dfs (int cfg[][MAXIMUM_VERTICES], int no_of_vertices, int p, int n) {
		
	int w, i;
	if (dfnum[n] == 0)	{
		dfnum[n] = N;
		vertex[N] = n;
		parent[n] = p;
		N++;
		for (i = 0; i < no_of_vertices; i++)	{
			w = cfg[n][i];
			if (w == 1 && i != 0)
				dfs(cfg, no_of_vertices, n, i);
		}
	}
}

void link(int p, int n)	{
	ancestor[n] = p;
}	

int ancestorWithLowestSemidominator (int v) {
	int u = v;	
	while (ancestor[v] != NONE)
	{
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
	
	/* fill up this function. */

	int i, s, tempS, y, n, p, v, j, w, k;
	for (i = 0; i < no_of_vertices; i++)	{
		idominator[i] = NONE;
		dfnum[i] = 0;
		semi[i] = NONE;
		ancestor[i] = NONE;
		samedom[i] = NONE;
	}
	dfs(cfg, no_of_vertices, NONE, 0);

	for (i = N - 1; i > 0; i--)	{
		n = vertex[i];
		p = parent[n];
		s = p;
		for (j = 0; j < no_of_vertices; j++)	{
			w = cfg[j][n];
			if (j != 0 && w == 1)
			{
				v = j;
				if (dfnum[v] <= dfnum[n])
					tempS = v;
				else
					tempS = semi[ancestorWithLowestSemidominator(v)];
				if (dfnum[tempS] < dfnum[s])
					s = tempS;
			}
		}
		semi[n] = s;
		for (k = 0; buckets[s][k] != 0; k++);
		buckets[s][k] = n; 
		link(p, n);
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
	for (i = 1; i < N; i++)	{
		n = vertex[i];
		if (samedom[n] != NONE)
			idominator[n] = idominator[samedom[n]];
	}
	return 0; 
	/* this function, as of now, just sets the immediate dominator for each node 'i' as 'i' itself. */
}
/* ----------------------------------------------------------------------------*/

