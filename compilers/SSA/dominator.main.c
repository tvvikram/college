#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "dominator.header.h"
#include "dominator.c"
#define DOT_OUTPUT

void print_dot_output (int idominator[], int no_of_vertices, const char *name, FILE *fp) {
	
	int i;	// index
	fprintf (fp, "digraph %s {\n", name);
	
	for (i = 0; i < no_of_vertices; i++)
		if (idominator[i] != NONE)
			fprintf (fp, "%d -> %d\n", idominator[i], i);
	
	fprintf (fp, "}\n");
}

void print_dot_input (int cfg[][MAXIMUM_VERTICES], int no_of_vertices, const char *name, FILE *fp) {
	
	int i, j;	// indices
	fprintf (fp, "digraph %s {\n", name);
	
	for (i = 0; i < no_of_vertices; i++)
		for (j = 0; j < no_of_vertices; j++)
			if (cfg[i][j] == 1)
				fprintf (fp, "%d -> %d\n", i, j);
	
	fprintf (fp, "}\n");
}

void print_cfg (int cfg[][MAXIMUM_VERTICES], int no_of_vertices) {
	
	int i, j;	// indices
	for (i = 0; i < no_of_vertices; i++) {
		for (j = 0; j < no_of_vertices; j++)
			printf ("%d ", cfg[i][j]);
		printf ("\n");
	}
}

int compare_output (int idominator1[], int idominator2[], int no_of_vertices) {
	
	int i;	// index
	for (i = 0; i < no_of_vertices; i++)
		if (idominator1[i] != idominator2[i]) {
			fprintf (stderr, "Dominator tree verification failed...\n");
			return false;
		}
	fprintf (stderr, "Dominator tree verification successful...\n");
	return true;
}

// to display idominator contents
int dispIDominator()
{
	int i;	// index
	printf("\nidominator: ");
	for (i = 0; i < NO_OF_NODES; i++)
		printf("%d\t", idominator[i]);
	printf("\n");
	return 0;
}

// to calculate the idominator of the nodes
int mainDominator (int no_of_vertices, int cfg[][MAXIMUM_VERTICES], int idominator[MAXIMUM_VERTICES]) {
	
	int i, j;	// indices
	FILE *input, *output;
	
	if (no_of_vertices > MAXIMUM_VERTICES) {
		fprintf (stderr, "Too many nodes..\n");
		abort();
	}
	
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
/*	
	// Print the dominator tree for CFG, as calculated during generation (verification) 
	output = fopen("dominator.gencfg.dot", "w");
	print_dot_output (idominator_from_gencfg, no_of_vertices, "DOMINATOR_TREE_FROM_GENCFG", output);
	
	// Compare results 
//	return_value = compare_output (idominator, idominator_from_gencfg, no_of_vertices);*/
#endif /* manual input */

#endif /* dot outout */
	
	return 0;
}
