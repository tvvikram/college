#include<stdio.h>
#include"token.h"

#define IDHEAD 301

#define INT 260
#define FL 261		// FLOAT
#define CH 262		// CHARACTER

#define IN 268		// INCLUDE

#define IF 270
#define EL 271		// ELSE
#define FR 275		// FOR

#define EQ 279
#define NE 280
#define EE 281
#define GT 282
#define GE 283
#define LT 284
#define LE 285

#define OF '{'		// OPENING FLOWER BRACKET
#define CF '}'		// CLOSING FLOWER BRACKET
#define OB '('		// OPENING BRACKET
#define CB ')'		// CLOSING BRACKET
#define SC ';'		// SEMI COLON

#define NI 290		// INTEGER NUMBER
#define NF 291		// FLOAT NUMBER
#define ID 295		// IDENTIFIER


// define the line number under current scan
// suitable for outputting errors also
int line = 1;

/* symbol table contains entries in the following way
        a global identifier and its value in string

        first 0 to 255 are reserved for ASCII definitions
        256 to 260 are currently blank
        261 to 299 are reserved for keywords, they are installed
                our language
        300 onwards are for the identifiers
*/

struct symTab
{
        int id;
        char *attribute;
        struct symTab * link;
};

typedef struct symTab * SYMTAB;

SYMTAB head = NULL;		// head of the symbol table
SYMTAB idHead = NULL;		// head of the symbol table from identifiers,
				// that is from id 300 onwards

// get a symbol table node
SYMTAB getSymTabNode()
{
	SYMTAB temp = (SYMTAB) malloc (sizeof (struct symTab));
	if (temp == NULL)
	{
		printf("Symbol table node cannot be created...\n");
		exit(1);
	}
	temp->id = -1;
	temp->attribute = NULL;
	temp->link = NULL;
	
	return temp;
}

// put values to the node of symbol table
SYMTAB putToSymTab (int id, char *attribute)
{
	SYMTAB node = getSymTabNode();
	node->id = id;
	node->attribute = (char *) malloc (sizeof (char)
					* (strlen (attribute) + 1));
	strcpy (node->attribute, attribute);
	
	return node;
}

// print the contents of symbol table
void printSymTab ()
{
	SYMTAB cur = head;
	
	while (cur != NULL)
	{
		printf("%d\t%s\n", cur->id, cur->attribute);
		cur = cur->link;
	}
	
	return;
}

// search for an attribute in symbol table
TOKEN lookUpSymTab(char *attribute)
{
	SYMTAB current = head;
	TOKEN token = getToken();
	while(current != NULL)
	{
		if (strcmp(attribute, current->attribute) == 0)
		{
			token->value = current->id;
			token->lexeme = 
				(char *) malloc (sizeof (char) * (strlen(attribute)+1));
			strcpy (token->lexeme, current->attribute);
			return token;
		}
		current = current->link;
	}
	return NULL;
}


// insertion can be done only from id with 300+ onwards
TOKEN insertToSymTab (int id, char *attribute)
{
	TOKEN token = getToken();
	token->value = id;
	token->lexeme = (char *) malloc (sizeof (char) * (strlen (attribute) + 1));
	strcpy (token->lexeme, attribute);

	if (id < 300)
	{
		printf("symbolTable.h: No permissions to insert...\n");
		exit(1);
	}
	
	if (lookUpSymTab (attribute) == NULL)
	{
		SYMTAB temp = putToSymTab (id, attribute), current;
		SYMTAB cur=head;
		if (idHead == NULL)
		{
			idHead = temp;
			while(cur->link)cur=cur->link;
			cur->link=idHead;
			return token;
		}
		current = idHead;
		while (current->link != NULL)
			current = current->link;
		current->link = temp;
		
		return token;
	}
	else
	{
		printf("symbolTable.h: Multiple declaration of %s.\n", attribute);
		return NULL;
	}
}

// install basic keywords in the symbol table
void installKeywords()
{
        SYMTAB current;
        
       	head = putToSymTab (INT, "int");

        current = putToSymTab (FL, "float");
        head->link = current;
        
	current->link = putToSymTab(CH,"char");
	current = current->link;

	current->link = putToSymTab(IN,"include");
	current = current->link;

	current->link = putToSymTab(IF,"if");
	current = current->link;

	current->link = putToSymTab(EL,"else");
	current = current->link;

	current->link = putToSymTab(FR,"for");
	current = current->link;

	current->link = putToSymTab(EQ,"=");
	current = current->link;

	current->link = putToSymTab(EE,"==");
	current = current->link;
	
	current->link = putToSymTab(NE,"!=");
	current = current->link;


	current->link = putToSymTab(GT,">");
	current = current->link;

	current->link = putToSymTab(GE,">=");
	current = current->link;


	current->link = putToSymTab(LT,"<");
	current = current->link;

	current->link = putToSymTab(LE,"<=");
	current = current->link;

	current->link = putToSymTab(OF,"{");
	current = current->link;
	
	current->link = putToSymTab(CF,"}");
	current = current->link;
	
	current->link = putToSymTab(OB,"(");
	current = current->link;
	
	current->link = putToSymTab(CB,")");
	current = current->link;
	
	current->link = putToSymTab(SC,";");
	current = current->link;
	
	
	return;
}

