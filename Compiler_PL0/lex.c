/* Ishmael Garcia
   HW4 - LEXICAL ANALYZER
   COP3402 - SUMMER 2022
   DR.MONTAGNE */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define MAX_IDENT_LEN 11
#define MAX_NUMB_LEN 5

lexeme *list;
char resWords[13][15] = { "begin", "end", "if", "then", "while", "do", "call", "const", "var", "procedure", "write", "read", "else" };
char specSym[9][3] = { ":=", "<=", ">=", "!=", "+=", "-=", "*=", "/=", "%="};
char specSymSingle[13] = { '+', '-', '*', '/', '%', '(', ')', ',', '.', ';', '=', '<', '>'};
int i,j;
int reswordval;
int ressymnum;
int reswordindex;
int lexindex;
int tempindex;
char readFile;
int skipnum;

void printerror(int tokenType);
void printtokens();
int isSymbol(char c);
int isResWord(char *c);
int isResSymbol(char *c);


lexeme *lexical_analyzer(char *input)
{
	char *c = malloc(500 * sizeof(char));
	list = malloc(500 * sizeof(lexeme));
	lexindex = 0;
	tempindex = 0;
	i = 0;
	reswordindex = 0;
	c = input; //THIS IS ADDED FROM CHANGES TO KEEP C ARRAY. ASSIGNED TO THE PASSED CHAR ARRAY INPUT

	//  loop through array
	i = 0;
	while (c[i] != '\0')
	{
		char* temp = malloc(15 * sizeof(char));
		tempindex = 0;

		// start letter check
		if (isalpha(c[i]))
		{
			temp[tempindex++] = c[i];
			i++;
			j = i;

			while (i - j <= 12 && !iscntrl(c[i]))
			{
				if (isSymbol(c[i]))
					break;
				temp[tempindex++] = c[i];
				i++;
			}

			// id length error
			if (i - j >= MAX_IDENT_LEN)
			{
				return NULL;
			}

			else if ((reswordval = isResWord(temp)) != -1)
			{
				list[lexindex].type = reswordval;
				lexindex++;
			}

			// must be a valid ID
			else
			{
				list[lexindex].type = identsym;
				strcpy(list[lexindex].name, temp);
				lexindex++;
			}
		}

		// first char is a symbol
		else if (isSymbol(c[i]))
		{

			if (isSymbol(c[i]) == -1)
			{
				if (iscntrl(c[i]) || isspace(c[i]))
				{
					i++;
					continue;
				}
				else
				{
					return NULL;
				}
			}

			temp[0] = c[i];
			if (c[i] != '\0' && !isdigit(c[i + 1]) && !isalpha(c[i + 1]) && !iscntrl(c[i + 1]))
			{
				temp[1] = c[i + 1];
				ressymnum = isResSymbol(temp);
				if (ressymnum)
				{
					list[lexindex].type = ressymnum;
					lexindex++;
					i += 2;
				}

				// start of a comment
				else if (strcmp(temp, "/*") == 0)
				{

					// we start reading in the comment
					while (c[i] != '\0')
					{
						if (c[i] == '*' && c[i + 1] == '/')	// if we break here, we reach end of comment.
							break;
						i++;
					}

					// if we reach the end of our input after start of comment
					if (c[i] == '\0')
					{
						return NULL;
					}
					else
					{
						i += 2;
						continue;
					}
				}
				// CHECKING FOR ! AND :
				else if ((c[i] == '!' || c[i] == ':') && (iscntrl(c[i+1]) || isspace(c[i+1])))
				{
					return NULL;
				}
				else if (isSymbol(c[i + 1]))
				{
					list[lexindex].type = isSymbol(c[i]);
					lexindex++;
					i++;
				}
				else
				{
					if (iscntrl(c[i]) || isspace(c[i]))
					{
						i++;
						continue;
					}
					else
					{
						return NULL;
					}
				}
			}
			else
			{
				list[lexindex].type = isSymbol(c[i]);
				lexindex++;
				i++;
			}
		}// end of symbol

		// first char is whitespace
		else if (iscntrl(c[i]) || isspace(c[i]))
		{
			i++;
			continue;
		}

		// first char is a number
		else if (isdigit(c[i]))
		{
			int tempNum[12];
			tempNum[tempindex++] = (int)c[i] - '0';
			i++;
			int j = i, output = 0;
			skipnum = 0;
			while (!iscntrl(c[i]))	// searching thru to get the number
			{
				if (isSymbol(c[i]))
					break;
				else if (isalpha(c[i]))
				{
					return NULL;
				}
				tempNum[tempindex++] = (int)c[i] - '0';
				i++;
			}	// now we have a potential number

			if (i - j > MAX_NUMB_LEN)
			{
				return NULL;
			}

			else if(skipnum != 2)	// must be a valid number
			{
				j = tempindex - 1;
				for (int z = 1; j >= 0; z *= 10)
				{
					output += (tempNum[j] * z);
					j--;
				}
				list[lexindex].type = numbersym;
				list[lexindex].value = output;
				lexindex++;
			}
		}// end of number
		free(temp);
	}

	printtokens();
	//free(c);
	return list;
}

// assuming only valid symbols
int isSymbol(char c)
{
	if (isdigit(c) || isalpha(c))
	{
		return 0;
	}
	if (c == ':' || c == '!')	// these arent specific values in my single char array, so I chose to do it manually
		return 1;

	for (int i = 0; i < 13; i++)
	{
		if (c == specSymSingle[i])
		{
			switch (i)
			{
			case 0:
				return plussym;
			case 1:
				return minussym;
			case 2:
				return multsym;
			case 3:
				return slashsym;
			case 4:
				return modsym;
			case 5:
				return lparentsym;
			case 6:
				return rparentsym;
			case 7:
				return commasym;
			case 8:
				return periodsym;
			case 9:
				return semicolonsym;
			case 10:
				return eqlsym;
			case 11:
				return lessym;
			case 12:
				return gtrsym;
			default:
				return -1;
			}// making sure i return the correct symval
		}// if it is a valid symbol
	}// end of loop
	return -1;
}

int isResSymbol(char* c)
{
	for (int i = 0; i < 9; i++)
	{
		if (strcmp(c, specSym[i]) == 0)
		{
			switch (i)
			{
			case 0:
				return becomessym;
			case 1:
				return leqsym;
			case 2:
				return geqsym;
			case 3:
				return neqsym;
			case 4:
				return addassignsym;
			case 5:
				return subassignsym;
			case 6:
				return timesassignsym;
			case 7:
				return divassignsym;
			case 8:
				return modassignsym;
			default:
				return -1;
			}// making sure i return the correct symval
		}// if it is a valid symbol
	}// end of loop
	return 0;	// if we get here, then our input was never a reserved symbol, but was stil 2 consec. symbols
}


// will return the appropriate token value for reserved words
int isResWord(char* c)
{
	for (int i = 0; i < 12; i++)
	{
		if (strcmp(c, resWords[i]) == 0)
		{
			return i + 21;
		}
	}
	if (strcmp(c, resWords[12]) == 0)
	{
		return 1;
	}
	return -1;		// indexed thru all resWords w/o returning, so I return -1 as a flag.
}

// print function
void printtokens()
{
	int i;
	printf("\nLexeme Table:\n");
	printf("\nlexeme\t\ttoken type\n\n");
	for (i = 0; i < lexindex; i++)
	{
		switch (list[i].type)
		{
			case eqlsym:
				printf("%s\t\t%d", "=", eqlsym);
				break;
			case neqsym:
				printf("%s\t\t%d", "!=", neqsym);
				break;
			case lessym:
				printf("%s\t\t%d", "<", lessym);
				break;
			case leqsym:
				printf("%s\t\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%s\t\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%s\t\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%s\t\t%d", "%", modsym);
				break;
			case multsym:
				printf("%s\t\t%d", "*", multsym);
				break;
			case slashsym:
				printf("%s\t\t%d", "/", slashsym);
				break;
			case plussym:
				printf("%s\t\t%d", "+", plussym);
				break;
			case minussym:
				printf("%s\t\t%d", "-", minussym);
				break;
			case lparentsym:
				printf("%s\t\t%d", "(", lparentsym);
				break;
			case rparentsym:
				printf("%s\t\t%d", ")", rparentsym);
				break;
			case commasym:
				printf("%s\t\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%s\t\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%s\t\t%d", ";", semicolonsym);
				break;
			case becomessym:
				printf("%s\t\t%d", ":=", becomessym);
				break;
			case beginsym:
				printf("%s\t\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%s\t\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%s\t\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%s\t\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%s\t\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%s\t\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%s\t\t%d", "do", dosym);
				break;
			case callsym:
				printf("%s\t\t%d", "call", callsym);
				break;
			case writesym:
				printf("%s\t\t%d", "write", writesym);
				break;
			case readsym:
				printf("%s\t\t%d", "read", readsym);
				break;
			case constsym:
				printf("%s\t\t%d", "const", constsym);
				break;
			case varsym:
				printf("%s\t\t%d", "var", varsym);
				break;
			case procsym:
				printf("%s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%s\t\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%d\t\t%d", list[i].value, numbersym);
				break;
			case addassignsym:
				printf("%s\t\t%d", "+=", addassignsym);
				break;
			case subassignsym:
				printf("%s\t\t%d", "-=", subassignsym);
				break;
			case timesassignsym:
				printf("%s\t\t%d", "*=", timesassignsym);
				break;
			case divassignsym:
				printf("%s\t\t%d", "/=", divassignsym);
				break;
			case modassignsym:
				printf("%s\t\t%d", "%=", modassignsym);
				break;
			default:
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lexindex; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n\n");
	//list[lexindex++].type = -1;
}

// error print function
void printerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length");
	else if (type == 5)
		printf("Lexical Analyzer Error: Neverending Comment");
	else
		printf("Implementation Error: Unrecognized Error Type");
	return;
}