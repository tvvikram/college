#include<stdio.h>
#include<stdlib.h>
#include"rbt.genOp.h"

RBT getrbnode()
{
	RBT p;
	if ((p = (RBT)malloc(sizeof (struct rbt))) == NULL)
	{
		printf("\nNo memory space allocated.\n");
		return;
	}
	printf("\nEnter info to be inserted: ");
	scanf("%d", &p->info);
	(p->llink) = p->rlink = NULL;
	(p->color) = 'R';
	return p;
}

RBT rbInsertFixup(RBT T, RBT z)								// z is the node just inserted and is red
{
	RBT y, m;
	while (z->plink != NULL && (z->plink)->color == 'R')	// if z's parent is also red, then there is a clash
	{
		// if z's parent is left child of its parent
		if (z->plink == ((z->plink)->plink)->llink)			
		{
			y = ((z->plink)->plink)->rlink;					// get y, uncle of z, here, right child of z's grandparent
			// if uncle is red, then
			if (y != NULL && y->color == 'R')				
			{
				(z->plink)->color = 'B';					// change both parent of z,
				(y->color) = 'B';							// uncle to black and
				z->plink->plink->color = 'R';				// grandparent of z to red
				z = z->plink->plink;
			}
			// if uncle is not red and
			else											
			{
				if (z == (z->plink)->rlink)					// if z is right child of its parent then
				{
					z = z->plink;							// move z to its parent and
					T = leftRotate(T, z);					// left rotate at z's parent
				}
				(z->plink)->color = 'B';					// z is left child of its parent, hence
				((z->plink)->plink)->color = 'R';			// color z's parent to black and grandparent to red, then
				T = rightRotate(T, (z->plink)->plink);		// right rotate at z's grandparent
			}
		}
		// if z's parent is right child of its parent
		else
		{
			y = ((z->plink)->plink)->llink;					// get y, uncle of z, here, left child of z's grandparent
			// if uncle is red
			if (y != NULL && y->color == 'R')
			{
				(z->plink)->color = 'B';					// change both parent of z,
				(y->color) = 'B';							// uncle to black and
				z->plink->plink->color = 'R';				// grandparent of z to red
				z = z->plink->plink;
			}
			// if uncle is not red and
			else
			{
				if (z == (z->plink)->llink)					// if z is left child of its parent then
				{
					z = z->plink;							// move z to its parent and
					T = rightRotate(T, z);					// right rotate at z's parent
				}
				(z->plink)->color = 'B';					// z is right child of its parent, hence
				((z->plink)->plink)->color = 'R';			// color z's parent to black and grandparent to red, then
				T = leftRotate(T, (z->plink)->plink);		// left rotate at z's grandparent
			}
		}
	}
	(T->color) = 'B';										// finally color the root node to black, not necessary for this code
	return T;
}

// to insert a node z into RBT
RBT rbInsert(RBT T, RBT z)					// same as BST insertion
{
	RBT y = NULL, x = T;
	if (T == NULL)							 
	{
		z->color = 'B';						// color of root to be black
		return z;
	}
	while (x != NULL)
	{
		y = x;
		if (z->info < x->info)
			x = x->llink;
		else
			x = x->rlink;
	}
	(z->plink) = y;
	if (y == NULL)		// not necessary for this code
		T = z;
	else
		if (z->info < y->info)
			y->llink = z;
		else
			y->rlink = z;
	return rbInsertFixup(T, z);				// fixUp RBT after insertion of a node
}

// to fixUp RBT after deletion of node
RBT rbDeleteFixUp(RBT t, RBT x)						// x was child of the spliced out node
{
	while (x != t && x->color == 'B')				// until x is root and its color is black
	{
		// if x is left child of its parent
		if (x == x->plink->llink)
		{
			RBT w = x->plink->rlink;				// w is right child of x's parent
			// if color of w is red then
			if (w != NULL && w->color == 'R')		
			{
				w->color = 'B';						// color it black
				x->plink->color = 'R';				// color x's parent to red and
				t = leftRotate(t, x->plink);		// left rotate at x's parent
				w = x->plink->rlink;				// move w to x's parent's right child
			}
			// if color of both left and right children of w are black then
			if (w != NULL && ((w->llink != NULL && w->llink->color == 'B') && (w->rlink != NULL && w->rlink->color == 'B')))
			{
				w->color = 'R';						// color w as red
				x = x->plink;						// move x to its parent
			}
			// if one of the children of w are red
			else 
			{
				// and if color of right child of w is black then
				if (w != NULL && w->rlink != NULL && w->rlink->color == 'B')
				{
					if (w->llink != NULL)
						w->llink->color = 'B';		// color left child of w as black,
					w->color = 'R';					// color w as red and
					t = rightRotate(t, w);			// right rotate at w
					if (x->plink != NULL)
						w = x->plink->rlink;		// move w to right child of x's parent
				}
				if (w != NULL && x->plink != NULL)
				{
					w->color = x->plink->color;		// color w as color of x's parent,
					x->plink->color = 'B';			// color x's parent as black and
					t = leftRotate(t, x->plink);	// left rotate at x's parent
				}
				if (w != NULL && w->rlink != NULL)
					w->rlink->color = 'B';			// color right child of w as black
				x = t;								// move x to root, end of fixUp after deletion
			}
		}
		// if x is right child of its parent
		else
		{
			RBT w = x->plink->llink;				// w is left child of x's parent
			// if color of w is red then
			if (w != NULL && w->color == 'R')
			{
				w->color = 'B';						// color it black
				x->plink->color = 'R';				// color x's parent to red and
				t = rightRotate(t, x->plink);		// right rotate at x's parent
				w = x->plink->llink;				// move w to x's parent's left child
			}
			// if color of both left and right children of w are black then
			if (w != NULL && ((w->llink != NULL && w->llink->color == 'B') && (w->rlink != NULL && w->rlink->color == 'B')))
			{
				w->color = 'R';						// color w as red
				x = x->plink;						// move x to its parent
			}
			// if one of the children of w are red
			else 
			{
				// and if color of left child of w is black then
				if (w != NULL && w->llink != NULL && w->llink->color == 'B')
				{
					if (w->rlink != NULL)
						w->rlink->color = 'B';		// color right child of w as black,
					w->color = 'R';					// color w as red and
					t = leftRotate(t, w);			// left rotate at w
					if (x->plink != NULL)
						w = x->plink->llink;		// move w to left child of x's parent
				}
				if (w != NULL && x->plink != NULL)
				{
					w->color = x->plink->color;		// color w as color of x's parent,
					x->plink->color = 'B';			// color x's parent as black and
					t = rightRotate(t, x->plink);	// right rotate at x's parent
				}
				if (w != NULL && w->llink != NULL)
					w->llink->color = 'B';			// color left child of w as black
				x = t;								// move x to root, end of fixUp after deletion
			}
		}
	}
	if (x!= NULL)
		x->color = 'B';								// finally color x as black
	return t;
}	

// to delete node z from RBT
RBT rbDelete(RBT t, RBT z)							// similar to BST deletion
{
	RBT y = NULL, x = NULL;
	if (z->llink == NULL || z->rlink == NULL)
		y = z;
	else
		y = successor(z);
	if (y->llink != NULL)
		x = y->llink;
	else
		x = y->rlink;
	if (x != NULL)
		x->plink = y->plink;
	if (y->plink == NULL)
		t = x;
	else if (y == y->plink->llink)
			y->plink->llink = x;
		else
			y->plink->rlink = x;
	if (y != z)
		z->info = y->info;
	if (y->color == 'B' && x != NULL)
		t = rbDeleteFixUp(t, x);								// delete fixUp only if the spliced node color is black
	return t;
}
