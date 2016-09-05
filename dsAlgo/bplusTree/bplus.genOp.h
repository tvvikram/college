#include<stdio.h>
#include<stdlib.h>
#define SIZE 3
#define SENTINEL 999999
#define DEBUG 0
#define DISPLAY_ALL_GRAPHS 0

struct bplus
{
	int keys[SIZE - 1];
	int leafNode;						// 1 if leafNode else 0
	struct bplus *children[SIZE];
	struct bplus *plink;
};
typedef struct bplus * BPLUS;

// writes to a file to display with dotty
void dispInDotty(BPLUS t, int c, FILE *fp)
{
	int m, i;
	static int count = 0;
	if (t == NULL)
		return;
	count++;
	m = findM(t);				// get number of pointers in the node
	fprintf(fp, "%d [shape=box label=\"Keys:", count);
	for (i = 0; i < m; i++)		// display all m - 1 keys
		if (t->keys[i] != 0)
			fprintf(fp, "%d ", t->keys[i]);
	fprintf(fp, "\"]");
	if (c != 0)
	fprintf(fp, "\n%d->%d\n", c, count);
/*	if (t->leafNode == 1)
		fprintf(fp, "\n%d->%d\n", c, ???);*/
	c = count;
	for (i = 0; i < m && t->leafNode == 0; i++)
		dispInDotty(t->children[i], c, fp);
	return;
}

/*void dispBP(BPLUS t)
{
	int i, m;
	if (t == NULL)
		return;
	m = findM(t);
	printf("\nKeys:");
	for (i = 0; i < m; i++)
		printf("\t%d", t->keys[i]);
	printf("\n");
	for (i = 0; i < m; i++)
		if (t->leafNode == 0)
			dispBP(t->children[i]);
	return;
}*/

void dispBPInDotty(BPLUS t)
{
	FILE *fp;
	static int i = 0;
	char filename[10], graph[50], command[50];
	sprintf (filename, "BP%d", i++);
	sprintf (graph, "%s.dot\0", filename);
	fp = fopen(graph, "w");
	fprintf(fp, "digraph bplus {\n");
	dispInDotty(t, 0, fp);
	fprintf(fp, "}");
	fclose(fp);
	sprintf (command, "dot -Tps %s -o %s.ps\0", graph, filename);
	system (command);
//	system ("rm *.dot");
	return;
}

void displayNode(BPLUS node)
{
	int m, i;
	if (node == NULL)
	{
		printf("\nNull node! Could not be displayed...\n");
		return;
	}
	m = findM(node);
	for (i = 0; i < m; i++)
		printf("%d\t", node->keys[i]);
	printf("LeafNode: %d\n", node->leafNode);
}

BPLUS getBPNode()
{
	int i;
	BPLUS temp = (BPLUS)malloc(sizeof(struct bplus));
	for (i = 0; i < SIZE; i++)
	{
		temp->keys[i] = SENTINEL;
		temp->children[i] = NULL;
	}
	temp->leafNode = 1;
	return temp;
}

int ceilX(float x)
{
	int i = (int) x;
	if (i == x)
		return i;
	return (i + 1);
}
		
int findChild(BPLUS parent, BPLUS child)
{
	int i, m = findM(parent);
	for (i = 0; i < m; i++)
		if (parent->children[i] == child)
			return i;
	return SIZE;
}

BPLUS moveMembers(BPLUS node, int m, int n)	// moves members from mth pointer at last till the nth pointer
{
	int i, j;
	int saveLeafNode = node->leafNode;
	BPLUS savePlink = node->plink;
	i = m;
	j = m;
	// if there are SIZE-1 pointers, then there are SIZE-2 keys
	if (m == SIZE - 1) j--;
	if (node->children[SIZE - 1] == NULL && m != SIZE - 1) i++;
	for ( ; i > n; i--, j--)
	{
		node->keys[j] = node->keys[j - 1];
		node->children[i] = node->children[i - 1];
	}
	node->plink = savePlink;
	node->leafNode = saveLeafNode;
	return node;
}

int findM(BPLUS node)						// to find the number of pointers in a node
{
	int i = 0, j = 0;
	if (node != NULL)
	{
//		printf("findM: Node is null.\n");
		if (node->leafNode == 1)
		{
			// if it is a leaf node, the last pointer has no importance
			while (i < (SIZE - 1) && node->children[i] != NULL)
				i++;
		}
		else if (node->leafNode == 0)
		{
			// else if it is a non-leaf node, the last pointer has importance
			while (i < (SIZE) && node->children[i] != NULL)
				i++;
		}
	}
	
	// to find the number of keys
	while (j < SIZE - 1 && node->keys[j] != SENTINEL)
		j++;
	
	if (i == j + 1)
		return i;
	else
		return j;
	
}

int findKey(BPLUS node, int key)			// to find key in the node
{
	int i = 0;
	if (node != NULL)
		while (i < SIZE)
		{
			if (node->keys[i] >= key)
				return i;
			i++;
		}
	return SIZE;
}

BPLUS findLeaf(BPLUS t, int key)		// to find the leaf node containing the key
{
	BPLUS c = t;
	int i, m;
	if (t == NULL)
		return NULL;
//	printf("c->keys[0] = %d, c->leafNode = %d\n", c->keys[0], c->leafNode);
	while (c != NULL && c->leafNode != 1)
	{
		i = findKey(c, key);
//		printf("\ni = %d\n", i);
		if (i == SIZE)
		{
			m = findM(c);
			c = c->children[m - 1];
		}
		else if (key < c->keys[i])
				c = c->children[i];
			else
				c = c->children[i + 1];
		
/*		else if (key < c->keys[i])
					c = c->children[i];
			 else if (key == c->keys[i])
					if (c->children[0]->leafNode == 1)
						c = c->children[i + 1];
					else
						c = c->children[i];
			else
				c = c->children[i + 1];*/
				
/*		else if (c->keys[i] >= key && c->children[0]->leafNode == 0)
					c = c->children[i];
				else
					c = c->children[i + 1];
//				else
	//				c = c->children[i - 1];
			
//		else
	//		c = c->children[i];*/
	}
	return c;
}

BPLUS find(BPLUS t, int key)			// to find the key in b+ tree, V = key
{
	int i;
	if (t == NULL)
		return NULL;
	BPLUS c = findLeaf(t, key);
	i = findKey(c, key);
	if (c->keys[i] == key)
		return c->children[i];
	else
		return NULL;
}
	
