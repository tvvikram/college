#include<stdio.h>
#include<stdlib.h>
#include"treap.insDel.h"

int countTraverse;

TREAP randomInsert(TREAP t)
{
	int n, check = 0;
	char c;
	struct timeval start, end;
	double runTime;
	FILE *fp;
	fp = fopen("r.dat", "w");
	printf("\nEnter number of nodes to randomly generate Red-Black Tree: ");
	scanf("%d", &n);
	printf("\nDo you want existing tree to be updated by random insertion? Press 'y' to continue...");
	scanf("%*c%c", &c);
	if (c == 'y' || c == 'Y')
	{
		gettimeofday(&start, NULL);
		t = genRandTREAP(t, n);
		gettimeofday(&end, NULL);
	}
	else
	{
		printf("Do you want existing tree to be deleted? Press 'y' to continue...");
		scanf("%*c%c", &c);
		if (c == 'y' || c == 'Y')
		{
			t = NULL;
			gettimeofday(&start, NULL);
			t = genRandTREAP(t, n);
			gettimeofday(&end, NULL);
			
		}					
		else
		{
			check = 1;
			printf("\nExisting tree retained.\n");
		}
	}
	if (!check)
	{
		runTime = (end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
		printf("\t%d\t%ld\n", n, runTime);
		fprintf(fp, "%d\t%ld\n", n, runTime);
	}
	fclose(fp);
	return t;
}

// traverse in preorder to find node at index 'n'
// a very costly algorithm!!!
TREAP nTraverse(TREAP t, int n)
{
	int i;
	TREAP temp = NULL;
	if (t == NULL || n == 0)				// return NULL if tree is NULL
		return NULL;
	countTraverse++;
	if (countTraverse == n)							// if node at index 'n' is found, return the node
	{
//		printf("countTraverse: %d, n: %d, Node: %d\n", countTraverse, n, t->info);
		return t;
	}
	temp = nTraverse(t->llink, n);			// recursively call with left children
	if (temp == NULL)						// only if there is no node at index at left subtree,
		temp = nTraverse(t->rlink, n);		// move to right subtree
	return temp;
}

TREAP autodel(TREAP t, int m)
{
	int i, n, check = 0;
	TREAP temp = NULL;
	srand((int)time(NULL));
//	printf("\nEnter the number of nodes to auto delete: ");
//	scanf("%d", &m);
	for (i = 0; i < m; i++)
	{
		n = (rand() % (m - i)) + 1;
		countTraverse = 0;
		if ((temp = nTraverse(t, n)) == NULL)
			printf("Node at index %d cannot be deleted!\n", n);
		else
		{
//			printf("%d-%c deleted.\n", temp->info, temp->color);
			t = rbDelete(t, temp);
//			dispTree(t);
			check++;
		}
	}
	printf("\t%d nodes deleted", check);
	return t;
}
