#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbolTable.h"
#include "lex.c"

void subList ();
void listGrammar ();
int keyword ();
void identifier ();
void factor ();
void subTerm ();
void term ();
void subExpr ();
void expr ();
void conditionGrammar ();
void basicStatement ();
void subStatement ();
void statementGrammar ();
void declarationGrammar ();
void bodyGrammar ();
void functionsGrammar ();
void includeGrammar ();
void program ();

void forGrammar ();
void elseGrammar ();
void ifGrammar ();

TOKEN lookahead;

// tells that a condition is following a basic statement
// if it is enabled then only one basic statement is parsed
int nextCond = 0;
int dottyParent=0;
int dottyNode=0;

FILE *dot;

void dispDotty(char *s)
{
	dottyNode++;
	if(strcmp(s,"\"")!=0)
		fprintf(dot,"\t%d[label=\"%s\"];\n",dottyNode,s);
	else 
		fprintf(dot,"\t%d[label=\"\\%s\"];\n",dottyNode,s);
		fprintf(dot,"\t%d->%d;\n",dottyParent,dottyNode);
	return;
}
void match (char *string)
{
	if (strcmp (string, lookahead->lexeme) == 0)
	{
//		printf("%s matched.\n", string);
		dispDotty(string);
		lookahead = lex();
		if (lookahead == NULL)
		{
			printf ("End of token stream...\n");
			fprintf(dot,"};\n");	
			fclose(dot);
			exit (0);
		}
		return ;
	}
	else
		// call error routine
		printf("line %d: Expected %s\n", line, string);
		
}

// subList : , identifier subList | e
void subList ()
{
	if (lookahead->value == ',')
	{
		match (",");
		identifier ();
		subList ();
	}
	else
		return;
}

// list    : identifier subList | e
// subList : , identifier subList | e
void listGrammar ()
{
	switch (lookahead->value)
	{
		case NI:
		case NF:
			identifier ();
			subList ();
			break;
		default:
			if (lookahead->value >= IDHEAD)
			{
				identifier ();
				subList ();
			}
			else
				return;
	}
}

// keyword : int | float | char | e
int keyword ()
{
	switch (lookahead->value)
	{
		case INT:
			match ("int");
			return 1;
		case FL:
			match ("float");
			return 1;
		case CH:
			match ("char");
			return 1;
		default:
//			printf ("line %d: parse.c: keyword: No keyword found...\n", line);
			return 0;
	}
}

void identifier ()
{
	switch (lookahead->value)
	{
		case NI:
		case NF:
			match (lookahead->lexeme);
			break;
		default:
			if (lookahead->value >= IDHEAD)
			match (lookahead->lexeme);
			else
			{
				printf ("line %d: parse.c: identifier: ", line);
				printf ("%s : Not a valid lexeme...\n", lookahead->lexeme);
				exit (1);
			}
	}
	return;
}

void factor ()
{
	if (lookahead->value == '(')
	{
		match ("(");
		expr ();
		match (")");
	}
	else
		identifier ();
}

void subTerm ()
{
	if (lookahead->value == '*')
	{
		match ("*");
		factor ();
		subTerm ();
	}
	else if (lookahead->value == '/')
	{
		match ("/");
		factor ();
		subTerm ();
	}
	else
		return;
}

void term ()
{
	factor ();
	subTerm ();
}
	

void subExpr ()
{
	if (lookahead->value == '+')
	{
		match ("+");
		term ();
		subExpr ();
	}
	else if (lookahead->value == '-')
	{
		match ("-");
		term ();
		subExpr ();
	}
	else
		return;
}

void expr ()
{
	term ();
	subExpr ();
}


// condition : identifier relCond identifier
void conditionGrammar ()
{
	int oldDottyParent=dottyParent;
	dispDotty("CONDITION");
	dottyParent=dottyNode;
	identifier ();
	switch (lookahead->value)
	{
		case EE:	match ("==");	break;
		case NE:	match ("!=");	break;
		case LE:	match ("<=");	break;
		case LT:	match ("<");	break;
		case GE:	match (">=");	break;
		case GT:	match (">");	break;
		case EQ:	match ("=");	break;
		default:
			{
				printf ("line %d: error: parse.c: cond: ", line);
				printf ("No relational operator matched...\n");
				exit (1);
			}
	}
	identifier ();
	dottyParent=oldDottyParent;
}

// basicStatement : identifier = expr ; basicStatement
//				  |	identifier (list) ; basicStatement
//				  | e
//				  | ;
void basicStatement ()
{
	if (lookahead->value == ';')
	{
		match (";");
		basicStatement ();
	}
	if (lookahead->value >= IDHEAD)
	{
		identifier ();
		// match a basic statement
		if (lookahead->value == '=')
		{
			match ("=");
			expr();
//			printf ("parse.c : basicStatement: Basic statement matched...\n");
		}
		// match a function call
		else if (lookahead->value == '(')
		{
			match ("(");
			listGrammar ();
			match (")");
			printf ("parse.c : basicStatement: Function call matched...\n");
		}
		match (";");
		if (!nextCond)
			basicStatement ();
	}
	else
		return;
}

// statement' : basicStatement | e
void subStatement ()
{
	basicStatement ();
	return;
}	

// statement  : if | for | statement'
// statement' : basicStatement | e
void statementGrammar ()
{
	switch (lookahead->value)
	{
		case IF:
			ifGrammar ();		break;
		case FR:
			forGrammar ();		break;
		default:
			subStatement ();	break;
	}
}


// declaration : keyword list ; declaration | e
void declarationGrammar ()
{
	// if keyword found, then look for list of identifiers declared
	if (keyword ())
	{
		listGrammar ();
		match (";");
		declarationGrammar ();
	}
	else
		return;
}	
			
// body : { statements } body | e
void bodyGrammar ()
{
	if (lookahead->value == '{')
	{
		int oldDottyParent=dottyParent;
		match ("{");
		dispDotty("BODY");
		dottyParent=dottyNode;
		declarationGrammar ();
		while (lookahead->value != '}')
			statementGrammar ();
		dottyParent=oldDottyParent;
		match ("}");
		bodyGrammar ();
	}
	else
		return;
}

// functions : keyword identifier (list) body functions | e
void functionsGrammar ()
{
	// if id is >= IDHEAD, then the lexeme under current scan
	// is an identifier
	if (lookahead != NULL && lookahead->value < IDHEAD)
	{
		keyword ();
		identifier ();
		match ("(");
		listGrammar ();
		match (")");
		bodyGrammar ();
		functionsGrammar ();
	}
	else
		return;
}

// include : include "identifier" include | e
void includeGrammar ()
{
	if (lookahead->value == IN)
	{
		match ("include");
		match ("\"");
		identifier ();
		match ("\"");
		printf ("line %d: parse.c: Include matched...\n", line);
		includeGrammar ();
	}
	else
		return;
}

// pragram : include functions
void program ()
{
	includeGrammar ();
	functionsGrammar ();
}

// FOR : for ( basicStatement condition ; basicStatement ) body
void forGrammar ()
{
	int oldDottyParent=dottyParent;
	int tParent;
	dispDotty("FOR LOOP");
	dottyParent=dottyNode;
	tParent=dottyNode;
	match ("for");
	match ("(");
	nextCond = 1;
	dispDotty("INITIALISATION");
	dottyParent=dottyNode;
	basicStatement ();
	dottyParent=tParent;
	nextCond = 0;
	conditionGrammar ();
	match (";");
	dispDotty("INCREMENT");
	dottyParent=dottyNode;
	basicStatement ();
	dottyParent=tParent;
	match (")");
	bodyGrammar ();
	dottyParent=oldDottyParent;
	printf ("line %d: parse.c: forGrammar matched...\n", line);
}

// ELSE : else body
void elseGrammar ()
{
	if (lookahead->value == EL)
	{
		int dottyOldParent=dottyParent;
		match ("else");
		dottyParent=dottyNode;
		bodyGrammar ();
		dottyParent=dottyOldParent;
		printf ("line %d: parse.c: elseGrammar matched...\n", line);
	}
	else
		return;
}

// IF   : if condition body ELSE
// ELSE : else body
void ifGrammar ()
{
	int dottyOldParent=dottyParent;
	dispDotty("IF");
	dottyParent=dottyNode;
	match ("if");
	match ("(");
	conditionGrammar ();
	match (")");
	bodyGrammar ();
	printf ("line %d: parse.c: ifGrammar matched...\n", line);
	elseGrammar ();
	dottyParent=dottyOldParent;
}

void parse()
{
	int id;
	TOKEN token;

/*	while(token=lex())
		printf("parse.c: %d\t%s\n",token->value,token->lexeme);*/
	
	// initialize lookahead
	lookahead = lex();
	
	// call the first production as program() until lookahead is null
//	while (lookahead != NULL)

	program ();

	return;
}

int main (int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		exit(0);
	}
	
	fp = fopen (argv[1], "r");
	installKeywords();

	dot=fopen("dotty.dot","w");
	fprintf(dot,"digraph parsetree{\n0[label=\"PARSE TREE\"]");	
	
	parse ();
	fprintf(dot,"};\n");	
	fclose(dot);
	return 0;
}

