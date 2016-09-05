#include<stdio.h>
#define MAX_SIZE 10		// maximum size of lexeme
				// hence maximum size of identifier can
				// be of size MAX_SIZE
FILE *fp;
char ch;	// a global character, also regarded as 
int id = IDHEAD;
int checkDelimit (char ch)
{
	// check if ch is within a-z or A-Z or 0-9
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
		|| (ch >= '0' && ch <= '9') || ch == '_')
		return 0;	// not delimiter
	else
		return 1;	// is a delimiter
}

// lexer
TOKEN lex ()
{
	int i = 0;
	
	// buffer size to store the lexemes of size MAX_SIZE
	char buffer[MAX_SIZE];
	TOKEN token = getToken();
	TOKEN retToken = NULL;

	do
	{
		ch = fgetc(fp);		
		if (ch == feof(fp)) break;
		
		if (ch == ' ' || ch == '\t')
			continue;
		else if (ch == '\n')
		{
			line++;
			continue;
		}
		
		if(checkDelimit(ch))
		{
			char lach = fgetc(fp);	// lookahead character
			// retract back 1 pointer as per the automata

			if (ch == feof(fp)) break;

			if (ch == '/' && lach == '*')
			{
				ch = fgetc (fp);
				lach = fgetc (fp);
				while (ch != '*' && lach != '/')
				{
					ch = lach;
					lach = fgetc (fp);
				}
				continue;
			}
			
			if (ch == '=' && lach == '=')
			{
				token->value = EE;
				token->lexeme = 
					(char *) malloc(sizeof(char) * 2);
				strcpy(token->lexeme, "==");
				return token;
			}
				
			if (ch == '<')
			{
				if (lach == '=')
				{
					token->value = LE;
					token->lexeme = 
						(char *) malloc(sizeof(char) * 2);
					strcpy(token->lexeme, "<=");
					return token;
				}
				else
				{
					token->value = LT;
					token->lexeme = 
						(char *) malloc(sizeof(char) );
					strcpy(token->lexeme, "<");
					fseek(fp, -1, SEEK_CUR);
					return token;
				}
			} // endof '<' scan
			
			if (ch == '>')
			{
				if (lach == '=')
				{
					token->value = GE;
					token->lexeme = 
						(char *) malloc(sizeof(char) * 2);
					strcpy(token->lexeme, ">=");
					return token;
				}
				else
				{
					token->value = GT;
					token->lexeme = 
						(char *) malloc(sizeof(char) );
					strcpy(token->lexeme, ">");
					fseek(fp, -1, SEEK_CUR);
					return token;
				}
			}
			
			if (ch == '!' && lach == '=')
			{
				token->value = NE;
				token->lexeme = 
					(char *) malloc(sizeof(char) * 2);
				strcpy(token->lexeme, "!=");
				return token;
			} // endof '>' scan
			
			// retract back one character from input stream
			
			if( ch == '+' || ch == '-' || ch == '*' || ch == '/' 
				|| ch == '%' || ch == '^' || ch == '}'
				|| ch == '{' || ch == '(' || ch == ')'
				|| ch == ';' || ch == ',' || ch == '='
				|| ch == '\"')
			{	
				token->value = ch;
				token->lexeme = 
					(char *) malloc(sizeof(char) * 2);	
				token->lexeme[0] = ch;
				token->lexeme[1] = '\0';
				fseek(fp, -1, SEEK_CUR);
				return token;
			} // endof operators scan
				ungetc(1, fp);
		} // endof checkdelimiter
	
		// scanning for numericals
		if (ch >= '0' && ch <= '9')
		{
			// flag = 0 is int, 1 is float
			int intFloatFlag = 0;
			while ((ch >= '0' && ch <= '9'))
			{
				buffer[i++] = ch;
				ch = fgetc(fp);
			}
			// check for float value
			if (ch == '.')
			{
				// check that there is a digit after '.'
				ch = fgetc(fp);
				if (ch >= '0' && ch <= '9')
				{
					intFloatFlag = 1;
					while (ch >= '0' && ch <= '9')
					{
						buffer[i++] = ch;
						ch = fgetc(fp);
						if (ch == feof(fp)) break;
					}
				}
				// else it is an error
				else
				{
					printf("line %d: lex.c: Not a valid float number...\n", line);
					fseek(fp, -1, SEEK_CUR);
					// call error routine
					continue;
				}
			}
			// if ch is not a '.' and not a delimiter, then its an error
			if (ch != '.' && !checkDelimit(ch))
			{
				printf("line %d: lex.c: Not a valid number...\n", line);
				fseek(fp, -1, SEEK_CUR);
				// call error routine
				continue;
			}
			// if there is a delimiter, it is an integer
			else if (checkDelimit(ch))
			{
				buffer[i] = '\0';
				if (intFloatFlag)	// is a float
					token->value = NF;
				else
					token->value = NI;
				token->lexeme = 
					(char *) malloc (sizeof(char) * i);
				strcpy (token->lexeme, buffer);
				i = 0;		// reset i
				fseek(fp, -1, SEEK_CUR);
				// return the integer token
				return token;
			}
		} // end of numerical

		// scanning identifiers and keywords
		
		// first character need to be letter and not a digit
		if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		{
			while (!checkDelimit(ch))
			{
				buffer[i++] = ch;
				ch = fgetc(fp);
			}
			if (ch == '.')
			{
				printf ("line %d: lex.c: Not a valid identifier... ", line);
				printf ("Cannot use '.' with an identifier. ");
				printf("Only '_' permitted...\n");
				continue;
			}
			buffer[i] = '\0';
			i = 0;
			fseek(fp, -1, SEEK_CUR);
	
			// check if buffer is in symbol table
			// symbol table returns a token if it is present
			// else it returns NULL and hence need to add buffer to symbol table
			// by incrementing the id
	
			retToken = lookUpSymTab (buffer);
	
			// insert buffer to symbol table if retId is -1
			if (retToken == NULL)
				// on inserting buffer to symbol table it returns an token
				return insertToSymTab (id++, buffer);
			else
				return retToken;
		}
		
	} // endof do-while loop
	while (ch != feof(fp));
	
	return NULL;
}
