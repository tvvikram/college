#include<stdio.h>
#include<stdlib.h>

struct rbt
{
	int info, count;
	struct rbt *llink, *rlink, *plink;	// left, right and parent links
	char color;	// B - black, R - red
};
typedef struct rbt *RBT;

// print Red Black Tree in Dotty
void dispInDotty(RBT t, FILE *fp)
{
	static int count = 0;
	if (t == NULL)
		return;
	count++;
	fprintf(fp, "\n%d [shape=circle ", t->count);
	if (t->color == 'B')
		fprintf(fp, "color=black ");
	else
		fprintf(fp, "color=red ");
	fprintf(fp, "label=\"%d\"]", t->info);
	if (t->plink != NULL)
		fprintf(fp, "\n%d->%d\n", t->plink->count, t->count);
	dispInDotty(t->llink, fp);
	dispInDotty(t->rlink, fp);
	return;
}

RBT minimum(RBT t)
{
	while (t->llink != NULL)
		t = t->llink;
	return t;
}

RBT maximum(RBT t)
{
	while (t->rlink != NULL)
		t = t->rlink;
	return t;
}

RBT successor(RBT x)
{
	if (x->rlink != NULL)
		return minimum(x->rlink);
	RBT y = x->plink;
	while (y != NULL && x == y->rlink)
	{
		x = y;
		y = y->plink;
	}
	return y;
}

RBT predecessor(RBT x)
{
	if (x->llink != NULL)
		return maximum(x->llink);
	RBT y = x->plink;
	while (y != NULL && x == y->llink)
	{
		x = y;
		y = y->plink;
	}
	return y;
}

RBT search(RBT t, int key)
{
	if (t == NULL || key == t->info)
		return t;
	if (key < t->info)
		return search(t->llink, key);
	else
		return search(t->rlink, key);
}

RBT leftRotate(RBT T, RBT x)			
{
	RBT y;
	y = x->rlink;							// y is right child of x
	if (y != NULL)							// to prevent segmentation faults
	{										// setting x and y's left child pointers
		// set right child of 'x' to left child of 'y'
		(x->rlink) = y->llink;
		// set parent of left child of 'y' to 'x'
		if (y->llink != NULL)
		(y->llink)->plink = x;
		// set the parent of 'y' to parent of 'x'
		(y->plink) = x->plink;				// setting y's parent as parent of x, if (y != NULL)
	
		// set child of x's parent to 'y'
		if (x->plink == NULL)
			T = y;
		else
			if (x == (x->plink)->llink)
				((x->plink)->llink) = y;	
			else
				((x->plink)->rlink) = y;
		// set left child of 'y' as 'x' and parent of 'x' as 'y'
		(y->llink) = x;						// setting y as parent of x, if (y != NULL)
		(x->plink) = y;
	}
	return T;
}

// 	x is node at which right rotation is to be done	
RBT rightRotate(RBT T, RBT x)			
{
	RBT y;
	y = x->llink;							// y is left child of x
	if (y != NULL)							// to prevent segmentation faults
	{										// setting x and y's right child pointers
		(x->llink) = y->rlink;
		if (y->rlink != NULL)
		(y->rlink)->plink = x;
		(y->plink) = x->plink;				// setting y's parent as parent of x, if (y != NULL)
		if (x->plink == NULL)
			T = y;
		else 
			if (x == (x->plink)->llink)
				((x->plink)->llink) = y;	
			else
				((x->plink)->rlink) = y;
		(y->rlink) = x;						// setting y as parent of x, if (y != NULL)
		(x->plink) = y;
	}
	return T;
}

// display tree in Graphical format
void dispTree(RBT T)
{
	static count;						// count for proper displacements while displaying
	int i;
	if (T == NULL)
		return;
	count++;
	dispTree(T->rlink);
	for (i = 0; i < count; i++)
		printf("\t");
	printf("%d-%c\n\n", T->info, T->color);
	dispTree(T->llink);
	count--;
}
