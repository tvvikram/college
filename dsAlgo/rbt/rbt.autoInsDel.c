#include<stdio.h>
#include<stdlib.h>
#include"rbt.insDel.h"
#define DEBUG 0

int count = 0;

// randomly generate n nodes for RBT
RBT genRandRBT(RBT t, int n)
{
	int i = 0;
	RBT temp = NULL;
	srand((int)time(NULL));
	for ( ; i < n; i++)
	{
		temp = (RBT)malloc(sizeof (struct rbt));			// allocate memory
		temp->llink = temp->rlink = temp->plink = NULL;		// initializing pointers
		temp->color = 'R';									// color newly created node as red
		if (DEBUG)
		{
			printf ("Enter info to insert: ");
			scanf ("%d", &temp->info);
		}
		else
			temp->info = rand() % 1000;							// randomly insert an info
		temp->count = ++count;
		t = rbInsert(t, temp);								// insert the created node
	}
	return t;
}

// recursively deletes the root of the RBT
RBT autodelRoot(RBT t, int m)
{
	int i, check = 0;
	RBT temp = NULL;
	for (i = 0; i < m; i++)
	{
		if (t == NULL)
			printf("\tTree empty\t");				// if RBT is empty
		else
		{
//			printf("%d-%d deleted.\n", t->info, t->priority);
			t = rbDelete(t, t);						// call to delete the root of the RBT
//			dispTree(t);
			check++;
		}
	}
	printf("\t%d nodes deleted", check);			// gives the number of nodes deleted
	return t;
}
