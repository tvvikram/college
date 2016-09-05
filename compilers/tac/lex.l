%{
	/* to scan the operators and lexemes */
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "y.tab.h"
	char *s;
%}

%%
"=="	{	
			s = (char *) malloc (strlen (yytext) + 1 + sizeof (char));
			strcpy (s, yytext);
			yylval.id = s;
			return EE;
		}
"<"	{	
			s = (char *) malloc (strlen (yytext) + 1 + sizeof (char));
			strcpy (s, yytext);
			yylval.id = s;
			return LT;
		}
"<="	{	
			s = (char *) malloc (strlen (yytext) + 1 + sizeof (char));
			strcpy (s, yytext);
			yylval.id = s;
			return LE;
		}
">"	{	
			s = (char *) malloc (strlen (yytext) + 1 + sizeof (char));
			strcpy (s, yytext);
			yylval.id = s;
			return GT;
		}
">="	{	
			s = (char *) malloc (strlen (yytext) + 1 + sizeof (char));
			strcpy (s, yytext);
			yylval.id = s;
			return GE;
		}
"!="	{	
			s = (char *) malloc (strlen (yytext) + 1 + sizeof (char));
			strcpy (s, yytext);
			yylval.id = s;
			return NE;
		}

[-=+*/!|&(),;]		{
						//	printf ("Operator: %s\n", yytext);
							return yytext[0];
						}
"true"					{
							return TRUE;
						}
"false"					{
							return FALSE;
						}
"if"					{
							return IF;
						}
"else"					{
							return ELSE;
						}
"while"					{
							return WHILE;
						}
[a-zA-Z_][a-zA-Z0-9_]*	{
						//	printf ("Identifier: %s\n", yytext);
							s = (char *) malloc (strlen (yytext) + 1
									+ sizeof (char));
							strcpy (s, yytext);
							yylval.id = s;
							return ID;
						}
[0-9][0-9]*				{
						//	 printf ("Integer: %s\n", yytext);
							s = (char *) malloc (strlen (yytext) + 1
									+ sizeof (char));
							strcpy (s, yytext);
							yylval.id = s;
							return INT;
						}
[ \t]					;

.						return yytext[0];
\n						{
						//	 printf ("Blankspace found\n");
						//	return yytext[0];
						}
%%

