%{
	#undef input
	#undef unput
	#include<stdio.h>
	#include<string.h>
	int input (void);
	void unput (int ch);
	static unsigned offset = 0;
	char **targv, **arglim;
	char *progName;
%}

%%
"./bplus.out"	{printf ("input");}

"--help"	{printf ("Helping... ;) "); return 0;}
%%

int yywrap ()
{
	return 1;
}

void lex (int argc, char **argv)
{
	progName = *argv;
	targv = argv + 1;
	arglim = argv + argc;
	yylex ();
	return;
}

int input (void)
{
	char c;
	if (targv >= arglim)
		return 0;
	
	if ((c = targv[0][offset++]) != '\0')
		return c;
	
	targv++;
	offset = 0;
	return (' ');
}

void unput (int ch)
{
	if (ch == 0)
		return;
	
	if (offset)
	{
		offset--;
		return;
	}
	
	targv--;
	offset = strlen (*targv);
}

