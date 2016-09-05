%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#include "header.h"
	#define NO_OF_TABLES 10
	#define NO_OF_ATTRIBUTES 10
	int tableNo = -1, i;
	int markIndex = 0;
	char words[30];
%}

%%
"create table" {	tableNo++; markIndex = 0; 
					for (i = 0; i < 13; i++, yytext++);
					for (i = 0; *yytext != '(' && *yytext != '\n'; i++, yytext++)
						words[i] = *yytext;
					words[i] = '\0';
					strcpy(tableNames[tableNo], words);
				}
"int" 	  {a[tableNo][markIndex] = 1; markIndex++;}
"float"   {a[tableNo][markIndex] = 2; markIndex++;}
"char" 	  {a[tableNo][markIndex] = 3; markIndex++;}
"varchar" {a[tableNo][markIndex] = 4; markIndex++;}
"date"	  {a[tableNo][markIndex] = 5; markIndex++;}
"\n\n\n"  {return;}
%%

int yywrap()
{
	return 1;
}

int createTableMatrix(char *filename)
{
	int i, j;
	yyin = fopen(filename, "r");
	yyout = fopen("/dev/null", "w");
	yylex();
	
	// to print the created matrix
/*	for (i = 0; i < NO_OF_TABLES; i++)
	{
		printf("\n");
		for (j = 0; j < NO_OF_ATTRIBUTES; j++)
			printf("%d\t", a[i][j]);
	}*/
	printf("\n");
	fclose(yyin);
	return 0;
}


