#include"bplus.genOp.h"

BPLUS insertInLeaf(BPLUS leaf, int key, BPLUS pointer)
{
	int mLeaf = findM(leaf);
	if (key < leaf->keys[0])
	{
		leaf = moveMembers(leaf, mLeaf, 0);
		leaf->keys[0] = key;
		leaf->children[0] = pointer;
	}
	else
	{
		int i = findKey(leaf, key);
		leaf = moveMembers(leaf, mLeaf, i);
		leaf->keys[i] = key;
		leaf->children[i] = pointer;
	}
	return leaf;
}

BPLUS insertInParent(BPLUS t, BPLUS node, int key, BPLUS splitNode)
{
	int mParent, iChild;
	BPLUS parent;
	if (node == t)
	{
		BPLUS newRoot = getBPNode();
		newRoot->keys[0] = key;
		newRoot->children[0] = node;
		node->plink = newRoot;
		newRoot->children[1] = splitNode;
		splitNode->plink = newRoot;
		newRoot->leafNode = 0;
		return newRoot;
	}
	parent = node->plink;
	mParent = findM(parent);
	iChild = findChild(parent, node);
//	dispBPInDotty(t);
	
	if (mParent < SIZE)
	{
		parent = moveMembers(parent, mParent, iChild);
		parent->keys[iChild] = key;
		parent->children[iChild + 1] = splitNode;
		splitNode->plink = parent;
		parent->leafNode = 0;
		return t;
	}
	else
	{
		int extraValue, ceiln, splitKey, i = 0, j = 0;
		BPLUS temp = getBPNode(), splitParent = getBPNode(), extraPointer;
		parent->leafNode = 0;
		// copy parent to temp
		for (i = 0; i < SIZE - 1; i++)
		{
			temp->keys[i] = parent->keys[i];
			temp->children[i] = parent->children[i];
		}
		temp->children[i] = parent->children[i];
		
		if (iChild < SIZE && temp->keys[SIZE - 2] > key)
		{
			extraPointer = temp->children[SIZE - 1];
			extraValue = temp->keys[SIZE - 2];
			temp = moveMembers(temp, mParent - 1, iChild);
			temp->keys[iChild] = key;
			temp->children[iChild + 1] = splitNode;
		}
		else
		{
			extraPointer = splitNode;
			extraValue = key;
		}
		
		// erase parent members
		for (i = 0; i < SIZE - 1; i++)
		{
			parent->children[i] = NULL;
			parent->keys[i] = SENTINEL;
		}
		parent->children[i] = NULL;
		// copy ceil(SIZE / 2) keys and pointers from temp to parent
		ceiln = ceilX(SIZE / 2.0);
		for (i = 0; i < ceiln; i++)
		{
			parent->children[i] = temp->children[i];
			if (parent->children[i] != NULL)
				parent->children[i]->plink = parent;
			parent->keys[i] = temp->keys[i];
		}
		// parent->children[i] = temp->children[i];
		// parent->children[i]->plink = parent;
		//parent->children[i] = NULL;
		splitKey = temp->keys[ceiln - 1];
//		splitParent->children[0] = temp->children[i];
//		splitParent->children[0]->plink = splitParent;
		// copy remaining members from temp to splitParent and from extra pointer and value
		for (j = 0; i < SIZE; i++, j++)
		{
			splitParent->children[j] = temp->children[i];
			if (splitParent->children[j] != NULL)
				splitParent->children[j]->plink = splitParent;
			if (i != SIZE - 1)
				splitParent->keys[j] = temp->keys[i];
		}
		splitParent->children[j] = extraPointer;
		if (splitParent->children[j] != NULL)
			splitParent->children[j]->plink = splitParent;
		if (i == SIZE)
			splitParent->keys[j - 1] = extraValue;
		parent->leafNode = 0;
		splitParent->leafNode = 0;
		t = insertInParent(t, parent, splitKey, splitParent);
		return t;
	}
}

BPLUS insertBPlus(BPLUS t, int value, BPLUS pointer)				// value is the key to be inserted
{
	BPLUS leaf, extraPointer;
	int mleaf, extraValue;
	if (t == NULL)
	{
		t = getBPNode();
		t->keys[0] = value;
		t->children[0] = pointer;
		if (DISPLAY_ALL_GRAPHS)
			dispBPInDotty(t);
		return t;
	}
	leaf  = findLeaf(t, value);
	mleaf = findM(leaf);
/*	if (mleaf == 0)
		printf("mleaf : %d\n", mleaf);
	if (leaf == NULL)
	{
		printf("Leaf is null!\n");
		printf("Number of pointers: %d.\n", mleaf);
		printf("Key to be inserted: %d.\n", value);
		dispBPInDotty(t);
		exit (0);
	}*/
	if (mleaf < SIZE - 1)
		leaf = insertInLeaf(leaf, value, pointer);
	else
	{
		int i, j, splitKey, ceiln;
		BPLUS splitLeaf = getBPNode();
		BPLUS temp = getBPNode();
		// copy leaf to temp
		for (i = 0; i < SIZE - 1; i++)
		{
			temp->keys[i] = leaf->keys[i];
			temp->children[i] = leaf->children[i];
		}
		temp->children[i] = leaf->children[i];
		if (value < temp->keys[SIZE - 2])
		{
			extraPointer = temp->children[SIZE - 2];
			extraValue = temp->keys[SIZE - 2];
			temp->children[SIZE - 2] = NULL;
			temp->keys[SIZE - 2] = SENTINEL;
			temp = insertInLeaf(temp, value, pointer);
		}
		else
		{
			extraPointer = pointer;
			extraValue = value;
		}
		splitLeaf->children[SIZE - 1] = leaf->children[SIZE - 1];
		leaf->children[SIZE - 1] = splitLeaf;
		
		// erase leaf members
		for (i = 0; i < SIZE - 1; i++)
		{
			leaf->children[i] = NULL;
			leaf->keys[i] = SENTINEL;
		}
		// copy ceil(SIZE / 2) keys and pointers from temp to leaf
		ceiln = ceilX(SIZE / 2.0);
		for (i = 0; i < ceiln; i++)
		{
			leaf->children[i] = temp->children[i];
			leaf->keys[i] = temp->keys[i];
		}
//		leaf->children[i] = temp->children[i];
		// copy remaining members from temp to splitLeaf and from extra pointer and value
		for (j = 0; i < SIZE - 1; i++, j++)
		{
			splitLeaf->children[j] = temp->children[i];
			splitLeaf->keys[j] = temp->keys[i];
		}
		splitLeaf->children[j] = extraPointer;
		splitLeaf->keys[j] = extraValue;
		splitKey = splitLeaf->keys[0];
		t = insertInParent(t, leaf, splitKey, splitLeaf);
	}
	if (DISPLAY_ALL_GRAPHS)
		dispBPInDotty(t);
	return t;
}
