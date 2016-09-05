#include<stdio.h>

struct token_t
{
	// token is identified by two parts - <tokenName, attribute>
	// tokenName is given by a value as given by the defines and
        // attribute is the lexeme itself
	int value;
	char *lexeme;
};

typedef struct token_t * TOKEN;

TOKEN getToken ()
{
	TOKEN temp;
	temp = (TOKEN) malloc (sizeof (struct token_t));
	if (temp == NULL)
	{
		printf("token.h: Could not create a token...\n");
		exit(1);
	}
	
	temp->value = -1;
	temp->lexeme = NULL;
	
	return temp;
}
