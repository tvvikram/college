#include<stdio.h>
#include<stdlib.h>
#include"bplus.insert.h"

int count = 0;	// to count the number of records that are being generated

struct record
{
	// creating bplus index on info
	int number;
	int info;
	struct record *link;
};
typedef struct record *RECORD;

// get an entry for the record
RECORD getEntry(FILE *fp)
{
	RECORD temp = NULL;
	temp = (RECORD)malloc(sizeof (struct record));
	if (DEBUG)
	{
	//	printf("\nEnter info to be inserted to record: ");
		scanf("%d", &temp->number);
	//	printf("\nEnter number: ");
		scanf("%d", &temp->info);
	}
	else
	{
		temp->number = rand() % SENTINEL + 1;
		temp->info = rand() % SENTINEL;
	}
	count++;
	fprintf(fp, "%d\t", temp->number);
	fprintf(fp, "%d\n", temp->info);
//	printf("Count: %d\t", count);
//	printf("\n");
	temp->link = NULL;
	
//	fprintf(fp, "%d\n%d\n", temp->number, temp->info);
	
	return temp;
}

// insert the entry into the records and update the B+ tree index
RECORD insertEntry(RECORD head, BPLUS *t, FILE *fp)
{
	int key;
	BPLUS pointer;
	RECORD temp = (RECORD)getEntry(fp), cur, prev;
	if (head->link == NULL)
	{
		head->link = temp;
		head->number++;
	}
	else
	{
		prev = head;
		cur = head->link;
		while (cur != NULL && cur->info < temp->info)
		{
			prev = cur;
			cur = cur->link;
		}
		if (temp->info == SENTINEL || (cur != NULL && cur->info == temp->info))		// check for duplicate entry
		{
//			printf("\nDuplicate entry!! %d is already inserted.\n", cur->info);
			return head;
		}
		temp->link = prev->link;
		prev->link = temp;
		head->number++;
	}
	key = temp->info;
	pointer = (BPLUS)temp;
//	printf(" and %d key.\n", key);
	*t = insertBPlus(*t, key, pointer);				// insert key, pointer to b+ tree; pointer is the pointer to inserted record

	return head;
}

// routine to display records
void displayRecords(RECORD head)
{
	int n = head->number;
	head = head->link;
	printf("Number\tInfo\n");
	while (n > 0)
	{
		printf("%d\t%d\n", head->number, head->info);
		head = head->link;
		n--;
	}
	return ;
}
