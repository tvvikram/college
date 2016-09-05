#include<stdio.h>
#include<stdlib.h>
#include "header.h"
#define INT_RANGE 1000
#define CHAR_RANGE 128
#define DOUBLE_RANGE 10000

// generate a random integer
int genInt()	{

	return rand() % INT_RANGE;
}

// generate a random double value
double genDouble()
{
	double x;
	x = (double)rand() / DOUBLE_RANGE;
	return x;
}

// checks for char generated
int checkChar(char n)
{
	if (n < 'A' || n > 'Z')
		return 1;
	switch (n)
	{
		case 'F':
		case 'J':
		case 'Q':
		case 'U':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':	return 1;
		default :	return 0;
	}
}

// generate a random integer in the ASCII range of capital letters
int genChar()	{

	int n;
	do
		n = rand() % CHAR_RANGE;
	while (checkChar(n));
	return n;
}
	
// generate 'n' tuple insertion formats and print to file
void genRecords(int n, int tableNo, FILE *fp)	
{
	int i, j;	// indices
	
	srand((int) time(NULL));
	// generate 'n' tuples
	for (i = 0; i < n; i++)
	{
		fprintf(fp, "insert into %s values (", tableNames[tableNo]);
		
		// generate one tuple by referring to the tableMatrix
		for (j = 0; a[tableNo][j] != -1; j++)
		{
			switch (a[tableNo][j])
			{
				case 1: // generate int
						fprintf(fp, "%d", genInt());					break;
				case 2: // generate float
						fprintf(fp, "%lf", genDouble());				break;
				case 3: // generate char, 2 characters will be generated
						fprintf(fp, "'%c%c'", genChar(), genChar());	break;
				case 4: // generate varchar, 4 characters will be generated
						fprintf(fp, "'%c%c%c%c'", genChar(), genChar(),
												  genChar(), genChar());break;
				case 5: // generate date in format dd/mm/yy
						// generating dates only till 28 to avoid more cases
						fprintf(fp, "'%d-%d-%d'",
						(genInt() % (2009 - 1900)) + 1900, (genInt() % 12) + 1,
						(genInt() % 28) + 1);							break;
			}
			if (a[tableNo][j+1] != -1)
				fprintf(fp, ", ");
		}
		fprintf(fp, ");\n");
	}
}

