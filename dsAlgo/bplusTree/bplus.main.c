#include<stdio.h>
#include<stdlib.h>
#include"bplus.records.h"

void menu()
{
	printf("\n 0. Menu 1. Insert Entry 2. Display Records 3. Search from B+ Tree 4. Save to display with Dotty\n");
}	
	
/*int main()
{
	int choice, l = 0;
	BPLUS t = NULL;
	RECORD head = (RECORD)malloc(sizeof (struct record));
	RECORD temp = NULL;
	FILE *fp = fopen("inputArgs", "w+");
	head->info = 0;
	head->number = 0;
	menu();
	do
	{
		printf("\nEnter choice: ");
		scanf("%d", &choice);
		switch(choice)
		{
			case 0: menu();							break;
			case 1:	head = (RECORD)insertEntry(head, &t, fp);	break;
			case 2: displayRecords(head);			break;
			case 3: printf("\nEnter info to search records using B+ Tree: ");
					scanf("%d", &choice);
					temp = (RECORD)find(t, choice);
					if (temp == NULL)
						printf("\nRecord not found!\n");
					else
						printf("Number: %d\nInfo: %d\n", temp->number, temp->info);	break;
			case 4: dispBPInDotty(t);						break;
			case 5: dispBP(t);								break;
			default:l = 1;
		}
	}
	while (l == 0);
	return 0;
}*/

RECORD insertRecords(int n, RECORD head, BPLUS *t)
{
	int i;
	FILE *fp = fopen("input", "w");
	// printf("Size is %d.\n", SIZE);
	// printf("\nEnter number of elements to generate: ");
	// scanf("%d", &n);
	srand((int)time(NULL));
	
	for (i = 0; i < n; i++)
	{
//		printf("Inserting %d element", i + 1);
		head = (RECORD)insertEntry(head, t, fp);

	}
	printf("%d records entered successfully.\n", i);
	return head;
}	

int main(int argc, char **argv)
{
	int n;
	BPLUS t = NULL;
	RECORD head = (RECORD)malloc(sizeof (struct record));
	RECORD temp = NULL;
	head->info = 0;
	head->number = 0;

//	lex (argc, argv);

	if (SIZE <= 1)
	{
		printf ("Impossible node structure!\n");
		exit (0);
	}
	
	if (!DEBUG)
	{
	
		if (argc != 2)
		{
			printf("Usage: %s (no. of elements).\n", argv[0]);
			exit(0);
		}
		else
			n = atoi(argv[1]);
	}
	else
		n = 65;		// for input with file `inputArgs' containing 65 entries
					// in DEBUG phase
	printf ("Size: %d\n", SIZE);
	
	system ("rm *.dot *.ps");
	
	printf("Number of elements: %d.\n", n);
	
	head = (RECORD)insertRecords(n, head, &t);

	printf("%d unique entries in record.\n", head->number);

	printf ("Generating graph...\n");
	dispBPInDotty(t);
//	displayRecords(head);

	free (head);
	free (t);
	return 0;
}
