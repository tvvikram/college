#include<stdio.h>
#include<stdlib.h>
#include"treap.insDel.h"

int count = 0;

// randomly generate n nodes for TREAP
TREAP genRandTreap(TREAP t, int n)
{
	int i = 0;
	TREAP temp;
	srand((int)time(NULL));
	for ( ; i < n; i++)
	{
		temp = (TREAP)malloc(sizeof (struct treap));		// allocate memory and initialize pointers
		temp->llink = temp->rlink = temp->plink = NULL;
		temp->info = rand()%1000;							// put a random info and
		temp->priority = rand()%10000;						// a random priority
		temp->count = ++count;
	//	printf("\nCreated: %d\t%d\n", temp->info, temp->priority);
		t = insert(t, temp);								// insert created node to the treap
	}
	return t;
}	

// recursively deletes the root of the treap
TREAP autodelRoot(TREAP t, int m)
{
	int i, check = 0;
	TREAP temp = NULL;
	for (i = 0; i < m; i++)
	{
		if (t == NULL)
			printf("\tTree empty\t");				// if treap is empty
		else
		{
//			printf("%d-%d deleted.\n", t->info, t->priority);
			t = delete(t, t);						// call to delete the root of the treap
//			dispTree(t);
			check++;
		}
	}
	printf("\t%d nodes deleted", check);			// gives the number of nodes deleted
	return t;
}

