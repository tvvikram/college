// Implementing fig. 6.36 and fig. 6.37
// Generation of 3 address code
%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "header.h"
	char * newTemp1 ();
	int newTemp ();
	char buf [100];
	int len;
	int i;
%}

%union
{
	int number;
	char *id;
	struct boolean *b;
	struct statement *s;
	struct expression *e;
};

%type <s> T
%type <s> S
%type <b> B
%type <e> E
%type <id> relop

%token <id> INT
%token <id> ID
%token <number> TRUE
%token <number> FALSE
%token <number> IF
%token <number> ELSE
%token <number> WHILE
%token <id> EE LT LE GT GE NE

%left '+' '-'
%left '*' '/'
%%
T	:	S
	{
		// S.next = newlabel ()
		// P.code = S.code || label (S.next)
	
		$$ = (struct statement *) malloc (sizeof (struct statement));
		len = strlen ($1->code) + 3;
		$$->code = (char *) malloc (len * sizeof (char));
		strcpy ($$->code, $1->code);
		sprintf ($$->code + strlen ($$->code), "L%d:\n \0", $1->next);
	
		printf ("%s\n", $$->code);
		return 0;
	}
	;
	
S		:ID '=' E ';'	// S.code = assign.code
	{
		$$ = (struct statement *) malloc (sizeof (struct statement));
		// S.code = E.code || "ID = E.addr"
		len = sprintf (buf, "%s = %s\n\0", $1, $3->addr);
		$$->code = (char *) malloc ((strlen ($3->code) + len) * sizeof (char));
		strcpy ($$->code, $3->code);
		strcat ($$->code, buf);

		// printf ("%s\n", $$->code);
	}
	|IF '(' B ')' S
	{
		$$ = (struct statement *) malloc (sizeof (struct statement));
		$5->next = $$->next = $3->f;
		// len = strlen ($3->code) + strlen ($5->code) + strlen ($7->code)
		//		+ strlen ($3->t) + strlen ($3->f) + ;
		// or randomly
		len = 1000;
		$$->code = (char *) malloc (len * sizeof (char));

		// S->code = B->code
		// || label (B->true) || S1->code

		// printf ("B.code: %s\n", $3->code);
		strcat ($$->code, $3->code);

		sprintf ($$->code + strlen ($$->code),
				"L%d:\n", $3->t);

	//		sprintf ($$->code + strlen ($$->code), "goto %d\n", $3->f);
		strcat ($$->code, $5->code);

		sprintf ($$->code + strlen ($$->code), "\0");
		// printf ("if:%sfi\n", $$->code);
	}
	|IF '(' B ')' S ELSE S
	{
		$$ = (struct statement *) malloc (sizeof (struct statement));
		$$->next = newLabel ();
		$5->next = $7->next = $$->next;
		// len = strlen ($3->code) + strlen ($5->code) + strlen ($7->code)
		//		+ strlen ($3->t) + strlen ($3->f) + ;
		// or randomly
		len = 1000;
		$$->code = (char *) malloc (len * sizeof (char));
		// S->code = B->code
		strcpy ($$->code, $3->code);

		// || label (B->true) || S1->code
		sprintf ($$->code + strlen ($$->code),
				"L%d:\n", $3->t);
		strcat ($$->code, $5->code);

		// || gen ('goto' S->next)
		sprintf ($$->code + strlen ($$->code),
				"goto L%d\n", $$->next);

		// || label (B->false) || S2->code
		sprintf ($$->code + strlen ($$->code),
				"L%d:\n", $3->f);
		strcat ($$->code, $7->code);
		
		sprintf ($$->code + strlen ($$->code),
				"L%d:\n", $$->next);

		sprintf ($$->code + strlen ($$->code), "\0");

	}
	|WHILE '(' B ')' S
	{
		int begin = newLabel ();
		$$ = (struct statement *) malloc (sizeof (struct statement));
		$$->next = $3->f;
		// $3->f = $$->next;
		$5->next = begin;
		len = 1500;
		$$->code = (char *) malloc (len * sizeof (char));
		// S.code = label (begin) || B.code || label (B.true)
		// || S1.code || "goto" begin
		
		sprintf ($$->code, "L%d: %sL%d:\n%sgoto L%d\n",
					begin, $3->code, $3->t, $5->code, begin);
	}
/*	| S S
	{
		$$ = (struct statement *) malloc (sizeof (struct statement));
		$1->next = newLabel ();
		$2->next = $$->next;
		len = 50;
		$$->code = (char *) malloc (len * sizeof (char));

		// S.code = S1.code
		strcpy ($$->code, $1->code);

		// || label (S1.next)
		sprintf ($$->code + strlen ($$->code), "%d ", $1->next);

		// || S2.code
		strcpy ($$->code, $2->code);
		sprintf ($$->code + strlen ($$->code), "\0");
	}*/
	;

B	:TRUE
	{
		$$ = (struct boolean *) malloc (sizeof (struct boolean));
				
		$$->t = newLabel ();
		$$->f = newLabel ();

		len = sprintf (buf, "goto L%d\n", $$->t);
		// printf ("buf: %s", buf);
		$$->code = (char *) malloc ((len + 1) * sizeof (char));
		strcpy ($$->code, buf);
		sprintf ($$->code + len, "\0");
		// printf ("B->code: %s", $$->code);
	}
	|FALSE
	{
			$$ = (struct boolean *) malloc (sizeof (struct boolean));
					
			$$->t = newLabel ();
			$$->f = newLabel ();
	
			len = sprintf (buf, "goto L%d\n", $$->f);
			// printf ("buf: %s", buf);
			$$->code = (char *) malloc ((len + 1) * sizeof (char));
			strcpy ($$->code, buf);
			sprintf ($$->code + len, "\0");
			// printf ("B->code: %s", $$->code);
	}
	| E relop E
	{
		$$ = (struct boolean *) malloc (sizeof (struct boolean));
		len = 1000;
		$$->code = (char *) malloc ((len + 1) * sizeof (char));
		// B.code = E1.code || E2.code
		// || "if E1.addr relop E2.addr goto B.true
		// || goto B.false

		$$->t = newLabel ();
		$$->f = newLabel ();

		strcpy ($$->code, $1->code);
		strcat ($$->code, $3->code);

		len = sprintf ($$->code + strlen ($$->code),
				"if %s %s %s goto L%d\ngoto L%d\n\0",
				$1->addr, $2, $3->addr, $$->t, $$->f);
	}
	| B '|' '|' B
	{
		$$ = (struct boolean *) malloc (sizeof (struct boolean));
		
		$$->t = newLabel ();
		$$->f = newLabel ();
		
		$1->t = $$->t;
		// $1->f = newLabel ();
		$4->t = $$->t;
		$4->f = $$->f;
		
		len = 1000;
		$$->code = (char *) malloc ((len + 1) * sizeof (char));
		sprintf ($$->code, "%s L%d:\n%s\n", $1->code, $1->f, $4->code);
	}
	| B '&''&' B
	{
		$$ = (struct boolean *) malloc (sizeof (struct boolean));
		
		// $1->t = newLabel ();
		$1->f = $$->f;
		$4->t = $$->t;
		$4->f = $$->f;
		
		len = 1000;
		$$->code = (char *) malloc ((len + 1) * sizeof (char));
		sprintf ($$->code, "%s L%d:\n%s\n", $1->code, $1->t, $4->code);
	}
	| '!' B
	{
		$$ = (struct boolean *) malloc (sizeof (struct boolean));
		$$->t = $2->f;
		$$->f = $2->t;
		
		len = 1000;
		$$->code = (char *) malloc ((len + 1) * sizeof (char));
		strcpy ($$->code, $2->code);
	}
	;

E	:	E '+' E	
{
	$$ = (struct expression *) malloc (sizeof (struct expression));
	// E.addr = new Temp ()
	$$->addr = (char *) malloc (3 * sizeof (char));
	sprintf ($$->addr, "t%d\0", newTemp ());

	// E.code = E1.code || E2.code || "E.addr = E1.addr + E2.addr"
	len = sprintf (buf, "%s = %s + %s\n", $$->addr, $1->addr, $3->addr);
	len += strlen ($1->code) + strlen ($3->code) + len + 1;
	// or randomly
	// len = 50
	$$->code = (char *) malloc (len * sizeof (char));
	strcpy ($$->code, $1->code);
	strcat ($$->code, $3->code);
	strcat ($$->code, buf);

	// printf ("%s\n", $$->code);
}
|	E '-' E	
{
	$$ = (struct expression *) malloc (sizeof (struct expression));
	// E.addr = new Temp ()
	$$->addr = (char *) malloc (3 * sizeof (char));
	sprintf ($$->addr, "t%d\0", newTemp ());

	// E.code = E1.code || E2.code || "E.addr = E1.addr - E2.addr"
	len = sprintf (buf, "%s = %s - %s\n", $$->addr, $1->addr, $3->addr);
	len += strlen ($1->code) + strlen ($3->code) + len + 1;
	// or randomly
	// len = 50
	$$->code = (char *) malloc (len * sizeof (char));
	strcpy ($$->code, $1->code);
	strcat ($$->code, $3->code);
	strcat ($$->code, buf);

	// printf ("%s\n", $$->code);
}
|	E '*' E	
{
	$$ = (struct expression *) malloc (sizeof (struct expression));
	// E.addr = new Temp ()
	$$->addr = (char *) malloc (3 * sizeof (char));
	sprintf ($$->addr, "t%d\0", newTemp ());

	// E.code = E1.code || E2.code || "E.addr = E1.addr * E2.addr"
	len = sprintf (buf, "%s = %s * %s\n", $$->addr, $1->addr, $3->addr);
	len += strlen ($1->code) + strlen ($3->code) + len + 1;
	// or randomly
	// len = 50
	$$->code = (char *) malloc (len * sizeof (char));
	strcpy ($$->code, $1->code);
	strcat ($$->code, $3->code);
	strcat ($$->code, buf);

	// printf ("%s\n", $$->code);
}
|	E '/' E	
{
	$$ = (struct expression *) malloc (sizeof (struct expression));
	// E.addr = new Temp ()
	$$->addr = (char *) malloc (3 * sizeof (char));
	sprintf ($$->addr, "t%d\0", newTemp ());

	// E.code = E1.code || E2.code || "E.addr = E1.addr / E2.addr"
	len = sprintf (buf, "%s = %s / %s\n", $$->addr, $1->addr, $3->addr);
	len += strlen ($1->code) + strlen ($3->code) + len + 1;
	// or randomly
	// len = 50
	$$->code = (char *) malloc (len * sizeof (char));
	strcpy ($$->code, $1->code);
	strcat ($$->code, $3->code);
	strcat ($$->code, buf);

	// printf ("%s\n", $$->code);
}
|	ID
{
	// printf ("ID: %s\n", $1);
	$$ = (struct expression *) malloc (sizeof (struct expression));
	// E.addr = top.get (id.lexeme)
	// E.code = ' '
	$$->addr = (char *) malloc ((strlen ($1) + 1) * sizeof (char));
	sprintf ($$->addr, "%s\0", $1);
	$$->code = (char *) malloc (1 * sizeof (char));
	sprintf ($$->code, "\0");
}
;

relop	:	EE | LT | LE | GT | GE | NE ;	
%%

int yywrap ()
{
	return 1;
}

int yyerror ()
{
	printf ("\nInvalid expression\n");
}

// returns a new unique label
// in terms of integers
int newLabel ()
{
	static int i = 0;
	i++;
	return i;
}

// just send the new temp as an integer
// concatenation is done in the calling function
int newTemp ()
{
	static int i = 0;
	i++;
	return i;
}

char * newTemp1 ()
{
	static int i = 0;	// static int that keeps generating new values
						// incrementally
	// create a buffer
	char *x = (char *) malloc (8 * sizeof (char));
	int n;		// gives the displacement till the end of integer from x
	sprintf (x, "t");
	n = sprintf (x + 1, "%d", ++i);
	// terminate the string
	sprintf (x + 1 + n, "\0");
//	printf ("in newTemp\n");
	return x;
}

FILE *yyin;

int main (int argc, char **argv)
{
	if (argc != 2)
	{
		printf ("Usage: %s <filename>\n", argv[0]);
		exit (0);
	}
	yyin = fopen (argv[1], "r");
	yyparse ();
}
