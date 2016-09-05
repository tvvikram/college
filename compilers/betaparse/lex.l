%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#include "y.tab.h"
%}

%%
"int"					{								/* Matches keyword int 	 	 */
							yylval.word = *yytext;					
							return TYPES;
						}
"float"					{								/* Matches keyword float 	 */
							yylval.word = *yytext;
							return TYPES;
						}
"char"					{								/* Matches keyword char 	 */
							yylval.word = *yytext;
							return TYPES;
						}
"test"					{
							yylval.word = *yytext;
							return TEST;
						}
"otherwise"				{
							yylval.word = *yytext;
							return OTHERWISE;
						}
"endt"					{
							yylval.word = *yytext;
							return ENDT;
						}
"loop"					{
							yylval.word = *yytext;
							return LOOP;
						}
"from"					{
							yylval.word = *yytext;
							return FROM;
						}
"step"					{
							yylval.word = *yytext;
							return STEP;
						}
"endl"					{
							yylval.word = *yytext;
							return ENDL;
						}
"endf"					{
							yylval.word = *yytext;
							return ENDF;
						}
"include"					{
							yylval.word = *yytext;
							return INCLUDE;
						}
"macro"					{
							yylval.word = *yytext;
							return MACRO;
						}
"endm"					{
							yylval.word = *yytext;
							return ENDM;
						}
"endp"					{
							yylval.word = *yytext;
							return ENDP;
						}						
"//".					return *yytext;
[A-Za-z][A-Za-z0-9]*	{								/* Matches a variable of     */
							yylval.word = *yytext - 'a';/* the form alphabets and    */
							return VARIABLE;			/* then digits and returns 	 */
						}								/* to VARIABLE of parse file */
[0-9]+					{								/* Matches a digit			 */
							yylval.number = atoi(yytext);
							return INTEGER;
						}
"++"					{								/* Matches increment operator*/
							yylval.word = *yytext;
							return AUTO;
						}
"--"					{								/* Matches decrement operator*/
							yylval.word = *yytext;
							return AUTO;
						}
"->"					{								/* Matches structure operator*/
							yylval.node = *yytext;
							return NODE;
						}
"=="					{return EQ;}						/* Matches conditional		 */
">="					{return GE;}						/* operators				 */
"<="					{return LE;}
"!="					{return NE;}
[-()\[\]<>=+*/;:^?{}.,]	return *yytext;					/* Matches operators		 */
"%"						return *yytext;
^"\n"					return;
"\n"					{return *yytext;}


%%

void yyerror(const char *msg)
{
	extern int errors;
	extern int errlines[50];
	errlines[errors]=yylineno;
	errors++;
	printf("betaparse: %d: %s at '%s'\n", yylineno, msg, yytext);
	return;
}
