%{
	#include<stdio.h>
	#include<stdlib.h>
	int valid = 1;
	int errors = 0;
	int errlines[50];
%}

%union {
	int number;
	char word;
	char node;
};

%token <word> VARIABLE
%token <word> AUTO 
%token <word> TYPES
%token <number> INTEGER
%token <node> NODE
%token EQ LE GE NE
%token <word> TEST
%token <word> OTHERWISE
%token <word> ENDT
%token <word> LOOP
%token <word> FROM
%token <word> STEP
%token <word> ENDL
%token <word> ENDF
%token <word> INCLUDE
%token <word> MACRO
%token <word> ENDM
%token <word> ENDP
%token <word> COMMENT

%%
full	:	program	endp	{valid = 0; return;}
		;

program	:	parts
		|	program parts
		|	func
		|	parts func
		|	program func
		|	program blank func
		|	error '\n' {yyerrok;}
		;

parts	:	include
		|	macro
		|	include macro
		|	parts include
		|	parts macro
		;

func	:	funcCall code endf
		;

include	:	INCLUDE VARIABLE blank
		;
		
macro	: 	MACRO VARIABLE code endm
		|	MACRO VARIABLE blank code endm
		;
		
code	:	statement
		|	relCond
		|	loopCond
		|	code statement
		|	code relCond
		|	code loopCond
		;

parameters:	VARIABLE
		|	parameters ',' VARIABLE
		;

loopCond:	LOOP condition statement endl
		|	LOOP condition FROM assign STEP assign statement endl
		|	LOOP condition FROM assign STEP assign blank statement endl
		;

relCond	:	TEST condition statement endt
		|	TEST condition statement OTHERWISE statement endt
		|	TEST condition statement OTHERWISE blank statement endt
		;

endt	:	ENDT
		|	ENDT blank
		|	error '\n' {yyerrok;}
		;

endl	:	ENDL
		|	ENDL blank
		|	error '\n' {yyerrok;}
		;
		
endf	:	ENDF
		|	ENDF blank
		|	error '\n' {yyerrok;}
		;

endm	: 	ENDM
		|	ENDM blank
		|	error '\n' {yyerrok;}
		;

endp	: 	ENDP
		|	error '\n' {yyerrok;}
		;

condition:	var cond var
		|	var cond var blank
		;

statement:	assign
		|	relCond
		|	loopCond
		|	assign '\n'
		|	blank
		|	statement blank
		|	funcCall ';'
		|	funcCall ';' '\n'
		|	statement funcCall ';'
		|	statement funcCall ';' '\n'
		|	statement assign
		|	statement assign '\n'
		|	statement relCond
		|	relCond statement
		|	statement loopCond
		|	loopCond statement
		|	error '\n' {yyerrok;}
		;

blank	: '\n'
		| blank '\n'
		;

funcCall:	VARIABLE '(' ')'
		|	VARIABLE '(' ')' '\n'
		|	VARIABLE '(' parameters ')'
		| 	VARIABLE '(' parameters ')' '\n'
		;

/* in all of the inputs semicolon (;) is a valid terminator */
assign :	auto ';'				/* Matches increment and decrement operations */
		|	expr ';'				/* Matches a simple expression */
		|	expr ',' assign			/* Matches expressions containing comma */
		|	expr '=' expr ';'		/* Matches multiple assignments (needs upgradation)*/
		|	TYPES expr ';'			/* Matches datatypes followed by expressions */
		|	TYPES expr ',' assign	/*  like int a = 0, b = c; also float, char */
		;

expr   : 	left right				/* Matches left and right part  of the assignment statement*/
		;
auto   :	var AUTO				/* Matches variable with post and pre increment and */
		|	AUTO var				/* post and pre decrement */
		;

left   : 	varL '='				/* Matches simple left */
		| 	varL '['option ']' '='
		| 	varL '['option ']''[' option ']' '='
		| 	varL '['option ']''[' option ']''['option ']' '='
		|	varL op '='				/* Matches compound assignment statements at left */
		|	varL str varL '='		/* Matches structure types at left */ 
		|	varL str varL op '='	/* Matches structure types with compound assignment */
		;						
			/* statements at left */
			
option: var
		| INTEGER
		;
right  :	var								/* Matches a variable at right */
		;


var	   :	auto							/* Matches increment and decrement operations at right */
		|	var op var 						/* Matches variable with operators at right */
		|	var str var						/* Matches structure at right */
		|	'('var cond var ')' '?' var ':' var	/* Matches tertiary operation */			
		|	'(' var ')'						/* Matches variable within small brackets */
		|	'(' varL ',' varL ')'
		|	'(' varL ',' varL ',' varL ')'
		|	op '('							/* Matches operator followed by opening small bracket */
		|	INTEGER							/* Matches integer as a variable */
		|	varL							/* Matches a simple variable */
		|	var '[' option ']'
		|	var '[' option ']''['option ']'
		|	var '[' option ']''['option ']''['option ']'
		;
			
varL   : 	VARIABLE				/* gets the variable scanned by the lexer */
		;
			
op	   :	'+'						/* gets operators: +, -, *, /, ^ from lexer */
		|	'-'
		|	'*'
		|	'/'
		|	'^'
		|	'%'
		;

cond   :	EQ						/* gets conditional operators: ==, <=, >=, !=, >, < from lexer */
		|	LE
		|	GE
		|	NE
		|	'>'
		|	'<'
		|	'='
		;
			
str	   :	NODE					/* Matches structure operators: -> and . */
		|	'.'
		;
%%


int yywrap()
{
	return 0;
}

extern FILE *yyin;
extern FILE *yyout;

int main(int argc, char **argv)
{	
	if( argc != 2 )
	{
		printf("Usage : betaparse (filename)\n");
		return 1;
	}
	yyin = fopen(argv[1], "r");
	yyout = fopen("/dev/null", "w");
//	printf("\nEnter expression: ");
	do
	{
		yyparse();
	}
	while (!feof(yyin));
	if (valid == 0)
	{
		printf("\nbetaparse: %d errors found!\n", errors);
		fclose(yyin);
	}
	else
	{
		printf("betaparse: Could not parse the file!! No function might exist!!\n");
		fclose(yyin);
	}
	dispDotty(errlines,errors,argv[1]);
	return valid;
}
