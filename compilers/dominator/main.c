#include <stdio.h>
#include <stdlib.h>
#include "header.h"

#define DOT_OUTPUT

void print_dot_output (int idominator[], int no_of_vertices, const char *name, FILE *fp) {
	
	int i;
	fprintf (fp, "digraph %s {\n", name);
	
	for (i = 0; i < no_of_vertices; i++)
		if (idominator[i] != NONE)
			fprintf (fp, "%d -> %d\n", idominator[i], i);
	
	fprintf (fp, "}\n");
}

void print_dot_input (int cfg[][MAXIMUM_VERTICES], int no_of_vertices, const char *name, FILE *fp) {
	
	int i, j;
	fprintf (fp, "digraph %s {\n", name);
	
	for (i = 0; i < no_of_vertices; i++)
		for (j = 0; j < no_of_vertices; j++)
			if (cfg[i][j] == 1)
				fprintf (fp, "%d -> %d\n", i, j);
	
	fprintf (fp, "}\n");
}

void print_cfg (int cfg[][MAXIMUM_VERTICES], int no_of_vertices) {
	
	int i, j;
	for (i = 0; i < no_of_vertices; i++) {
		for (j = 0; j < no_of_vertices; j++)
			printf ("%d ", cfg[i][j]);
		printf ("\n");
	}
}

int compare_output (int idominator1[], int idominator2[], int no_of_vertices) {
	
	int i;
	
	for (i = 0; i < no_of_vertices; i++)
		if (idominator1[i] != idominator2[i]) {
			fprintf (stderr, "Dominator tree verification failed...\n");
			return false;
		}
	fprintf (stderr, "Dominator tree verification successful...\n");
	return true;
}

int main (int argc, char **argv) {
	
	int cfg[MAXIMUM_VERTICES][MAXIMUM_VERTICES], i, j, no_of_vertices;
	int idominator[MAXIMUM_VERTICES], idominator_from_gencfg[MAXIMUM_VERTICES];
	int idominator2[MAXIMUM_VERTICES], return_value = EXIT_SUCCESS;
	FILE *input, *output;
	int m, n;
	/* we don't need idominator2[] as we can reuse idominator itself */
	
	if (argc != 2) {
		fprintf (stderr, "Usage: %s n\nEnter the graph matrix here...\n", argv[0]);
		abort();
	}
	
	no_of_vertices = atoi (argv[1]);
	
	if (no_of_vertices > MAXIMUM_VERTICES) {
		fprintf (stderr, "Too many nodes..\n");
		abort();
	}
	
/*#ifdef MANUAL_INPUT	
	// takes the graph matrix from the standard intput
	for (i = 0; i < no_of_vertices; i++) for (j = 0; j < no_of_vertices; j++)
		scanf ("%d", &cfg[i][j]);
#else*/
	generate_cfg (cfg, no_of_vertices, idominator_from_gencfg);
// #endif
/*
//	printf("\nEnter cfg... How many nodes: ");
//	scanf("%d", &i);
	printf("Enter now:\n");
	for (m = 0; m < no_of_vertices; m++)
		for (n = 0; n < no_of_vertices; n++)
			scanf("%d", &cfg[m][n]);
	
	printf("You entered the cfg:\n");
	for (m = 0; m < no_of_vertices; m++)
	{
		printf("\n");
		for (n = 0; n < no_of_vertices; n++)
			printf("%d\t", cfg[m][n]);
	}*/
	
	build_dominator_tree (cfg, idominator, no_of_vertices);
	
#ifndef DOT_OUTPUT
	printf ("\n");
	for (i = 0; i < no_of_vertices; i++)
		printf ("%d ", idominator[i]);
	printf ("\n");
#else
	input = fopen("cfg.dot", "w");
	print_dot_input (cfg, no_of_vertices, "CFG", input);
	output = fopen("dominator.lengauerTarjan.dot", "w");
	print_dot_output (idominator, no_of_vertices, "DOMINATOR_TREE_FROM_LENGAUER_TARJAN", output);

#ifndef MANUAL_INPUT
	
	/* Print the dominator tree for CFG, as calculated during generation (verification) */
//	output = fopen("dominator.gencfg.dot", "w");
//	print_dot_output (idominator_from_gencfg, no_of_vertices, "DOMINATOR_TREE_FROM_GENCFG", output);
	
	/* Compare results */
//	return_value = compare_output (idominator, idominator_from_gencfg, no_of_vertices);
#endif /* manual input */

#endif /* dot outout */
	
	return !return_value;
}
