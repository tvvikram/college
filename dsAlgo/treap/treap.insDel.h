#include<stdio.h>
#include<stdlib.h>
#include"treap.genOp.h"

// to fixup the tree after insertions
TREAP insertFixUp(TREAP t, TREAP temp)
{
	while (temp != NULL && temp->plink != NULL && temp->priority < temp->plink->priority)
	{
		if (temp == temp->plink->llink)					// if temp is left child of its parent
			t = rightRotate(t, temp->plink);			// right rotate at its parent to make temp as parent
		else	// if (temp = temp->plink->rlink)		// so that priorities are satisfied as min heap
			t = leftRotate(t, temp->plink);				// else left rotate
	}													// to satisfy min heap property
	return t;
}

// insert a new treap node with random info and random priority, a BST Insert
TREAP insert(TREAP T, TREAP z)
{
	TREAP y = NULL, x = T;
	if (T == NULL)
		return z;
	while (x != NULL)							// insert at a suitable position, referenced with BST
	{
		y = x;
		if (z->info < x->info)
			x = x->llink;
		else
			x = x->rlink;
	}
	z->plink = y;
	if (z->info < y->info)
		y->llink = z;
	else
		y->rlink = z;
	
	 return insertFixUp(T, z);		
}

// delete treap node only when it becomes a leaf node
TREAP delete(TREAP t, TREAP z)							// z is node to be deleted
{
	while (z->llink != NULL || z->rlink != NULL)		// until z becomes leaf
	{
		if (z->rlink == NULL)							// checking for non-rotation positions
			t = rightRotate(t, z);						// as there is no right rotation when there its right pointer is NULL
		if (z->llink == NULL)
			t = leftRotate(t, z);
		if (z->plink != NULL && z == z->plink->llink)	// if z is left child of its parent then right rotate at it
			t = rightRotate(t, z);
		else											// else left rotate at it
			t = leftRotate(t, z);
	}
	if (z->llink == NULL && z->rlink == NULL)			// if z is leaf, then delete it
	{
		if (z->plink == NULL)							// if z is the only root node, return NULL
			return NULL;
		else
		{
			if (z == z->plink->llink)					// else delete it
				z->plink->llink = NULL;
			else
				z->plink->rlink = NULL;
			free(z);
		}
	}
	return t;
}
