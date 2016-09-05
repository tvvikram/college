#include<stdio.h>
#include<stdlib.h>

struct treap
{
	int info, priority, count;
	struct treap *llink, *rlink, *plink;	// setting left child, right child and parent links
};
typedef struct treap *TREAP;

// print Red Black Tree in Dotty
void dispInDotty(TREAP t, FILE *fp)
{
	static int count = 0;
	if (t == NULL)
		return;
	count++;
	fprintf(fp, "\n%d [shape=box ", t->count);
	if (t->plink != NULL)
		if(t == t->plink->llink)
			fprintf(fp, "color=blue ");
		else
			fprintf(fp, "color=red ");	
	fprintf(fp, "label=\"Key:%d\\nPriority:%d\"]", t->info, t->priority);
	if (t->plink != NULL)
		fprintf(fp, "\n%d->%d\n", t->plink->count, t->count);
	dispInDotty(t->llink, fp);
	dispInDotty(t->rlink, fp);
	return;
}

// x is node at which left rotation is to be done
TREAP leftRotate(TREAP T, TREAP x)			
{
	TREAP y;
	y = x->rlink;							// y is right child of x
	if (y != NULL)							// to prevent segmentation faults
	{										// setting x and y's left child pointers
		(x->rlink) = y->llink;
		if (y->llink != NULL)
		(y->llink)->plink = x;
		(y->plink) = x->plink;				// setting y's parent as parent of x, if (y != NULL)
	
		if (x->plink == NULL)
			T = y;
		else
			if (x == (x->plink)->llink)
				((x->plink)->llink) = y;	
			else
				((x->plink)->rlink) = y;
		(y->llink) = x;						// setting y as parent of x, if (y != NULL)
		(x->plink) = y;
	}
	return T;
}

// 	x is node at which right rotation is to be done	
TREAP rightRotate(TREAP T, TREAP x)			
{
	TREAP y;
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
void dispTree(TREAP T)
{
	static count;
	int i;
	if (T == NULL)
		return;
	count++;								// count for proper displacements
	dispTree(T->rlink);
	for (i = 0; i < count; i++)
		printf("\t");
	printf("%d-%d\n\n", T->info, T->priority);
	dispTree(T->llink);
	count--;
}

TREAP getnode()
{
  TREAP temp = (TREAP)malloc(sizeof (struct treap));
  printf("\nEnter info to insert: ");
  scanf("%d", &temp->info);
  printf("\nEnter priority to insert: ");
  scanf("%d", &temp->priority);
  printf("\n%d-%d will be inserted.\n", temp->info, temp->priority);
  temp->llink = NULL;
  temp->rlink = NULL;
  temp->plink = NULL;
  return temp;
}

TREAP search(TREAP t, int key)
{
  if (t == NULL || key == t->info)
    return t;
  if (key < t->info)
    return search(t->llink, key);
  else
    return search(t->rlink, key);
}
