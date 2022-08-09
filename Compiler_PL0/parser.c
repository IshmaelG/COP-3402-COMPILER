/*
	skeleton for Dr. Montagne's Systems Software project Summer 2022
	you may alter all of this code if desired, but you must provide clear
	instructions if you alter the compilation process or wish us to
	use different files during grading than the ones provided 
	(driver.c, compiler.h, vm.o/.c, or lex.o/.c)
*/

/* Ishmael Garcia (3526912)
   HW4 - Parser
   COP3402 - SUMMER 2022
   DR.MONTAGNE */


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

lexeme *tokens;
int lIndex = 0;
instruction *code;
int cIndex = 0;
symbol *table;
int tIndex = 0;



int level;

int i = 0;
int symidx;

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void mark();
int multipledeclarationcheck(char name[]);
int findsymbol(char name[], int kind);

void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

void block(lexeme *list);
void constDec(lexeme *list);
int varDec(lexeme *list);
void procDec(lexeme *list);
void statement(lexeme *list);
void condition(lexeme *list);
void expression(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);

instruction *parser_code_generator(lexeme *list)
{
	tokens = list;
	code = malloc(sizeof(instruction) * MAX_ARRAY_SIZE);
	table = malloc(sizeof(symbol) * MAX_ARRAY_SIZE);
	i = 0;
	// PROGRAM

	emit(7, 0, 0);
	addToSymbolTable(3, "main", 0, 0, 0, 0);
	level = -1;
	block(list);

	//period: ERROR 1
	if (list[i].type != periodsym)
	{
		printparseerror(1);
		return NULL;
	}

	// printf("HERE IS WHERE WE ARE AT: %d", list[i].type);
	emit(9, 0, 3);
	code[0].m = table[0].addr * 3;
	for(int ci = 0; ci < cIndex; ci++)
	{
		if (code[ci].op == 5)
		{
			code[ci].m = table[code[ci].m].addr * 3;
		}
	}

	// PROGRAM END

	printsymboltable();
	printassemblycode();
	
	code[cIndex].op = -1;
	return code;
}

// adds an instruction to the end of the code array
void emit(int opname, int level, int mvalue)
{
	code[cIndex].op = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

// adds a symbol to the end of the symbol table
void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].value = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

// starts at the end of the table and works backward (ignoring already
// marked entries (symbols of subprocedures)) to mark the symbols of
// the current procedure. it knows it's finished the current procedure
// by looking at level: if level is less than the current level and unmarked
// (meaning it belongs to the parent procedure) it stops
void mark()
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

// does a linear pass through the symbol table looking for the symbol
// who's name is the passed argument. it's found a match when the names
// match, the entry is unmarked, and the level is equal to the current
// level. it returns the index of the match. returns -1 if there are no
// matches
int multipledeclarationcheck(char name[])
{
	int i;
	for (i = 0; i < tIndex; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

// returns -1 if a symbol matching the arguments is not in the symbol table
// returns the index of the desired entry if found (maximizing the level value)
int findsymbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using ':=, +=, -=, *=, /=, or %%=' \n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].value, table[i].level, table[i].addr, table[i].mark); 
	printf("\n");
}

void printassemblycode()
{
	int i;
	printf("Assembly Code:\n");
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].op);
		switch (code[i].op)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("MOD\t");
						break;
					case 7:
						printf("EQL\t");
						break;
					case 8:
						printf("NEQ\t");
						break;
					case 9:
						printf("LSS\t");
						break;
					case 10:
						printf("LEQ\t");
						break;
					case 11:
						printf("GTR\t");
						break;
					case 12:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				printf("SYS\t");
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	printf("\n");
}

void block(lexeme *list)
{
	level++;
	int pindex = tIndex-1;
	constDec(list);
	int varnum = varDec(list);
	procDec(list);
	table[pindex].addr = cIndex;
	emit(6, 0, varnum + 3); //***CHANGED LEVEL
	statement(list);
	mark();
	level--;
}

void constDec(lexeme *list)
{
	int identname = 0;
	// check const
	if (list[i].type == constsym)
	{
		// if const then loop while comma
		do
		{
			// next token + ident check
			i++;
			if (list[i].type == identsym)
			{
				if (multipledeclarationcheck(list[i].name) != -1)
				{
					printparseerror(18);
					exit(0);
				}

				// save ident index
				identname = i;

				// next token + equal check
				i++;
				
				if (list[i].type == becomessym)
				{
					i++;
					if (list[i].type == numbersym)
					{
						// if no error then add to symbol table
						addToSymbolTable(1, list[identname].name, list[i].value, level, 0, 0);
					}
					// ERROR 2: NO NUM
					else
					{
						printparseerror(2);
						exit(0);
					}
				}
				// ERROR 2: NO becomesym
				else
				{
					printparseerror(2);
					exit(0);
				}
			}
			// ERROR 2: NO IDENT
			else
			{
				printparseerror(2);
				exit(0);
			}
			i++;
		} while (list[i].type == commasym);

		if (list[i].type == semicolonsym)
		{
			i++;
		}
		else if (list[i].type == identsym)
		{
			printparseerror(13);
			exit(0);
		}
		else
		{
			printparseerror(14);
			exit(0);
		}
	}
}

int varDec(lexeme *list)
{
	// dec variable to count # of variables
	int dec = 0;
	if (list[i].type == varsym)
	{
		do
		{
			// goes to next token and checks ident
			i++;
			if (list[i].type == identsym)
			{
				addToSymbolTable(2, list[i].name, 0, level, dec + 3, 0);
				i++;
				dec++;
			}
			// error 3 check: no ident
			else
			{
				printparseerror(3);
				exit(0);
			}
		} while (list[i].type == commasym);

		if (list[i].type == semicolonsym)
		{
			i++;
			return dec;
		}
		else if (list[i].type == identsym)
		{
			printparseerror(13);
			exit(0);
		}
		else
		{
			printparseerror(14);
			exit(0);
		}
	}
	else
		return 0;
}

void procDec(lexeme *list)
{
	// procedure loop
	while (list[i].type == procsym)
	{
		// next token + check ident
		i++;
		if (list[i].type == identsym)
		{
			// next token + semicolon check
			i++;
			if (list[i].type == semicolonsym)
			{
				// dont know addr yet
				// add to table + next token
				addToSymbolTable(3, list[i - 1].name, 0, level, 0, 0);
				i++;
				block(list);
				// semicolon check
				if (list[i].type == semicolonsym)
				{
					emit(2, 0, 0);
					i++;
				}
				else if (list[i].type != identsym)
				{
					printparseerror(14);
					exit(0);
				}
			}
			// ERROR 4: NO SEMICOLON BEFORE BLOCK
			else
			{
				printparseerror(4);
				exit(0);
			}
		}
		// no ident error
		else
		{
			printparseerror(4);
			exit(0);
		}
	}
}

void statement(lexeme *list)
{
	int jpcidx;
	int jmpidx;
	int loopidx;

	// ASSIGNMENT STATEMENT

	if (list[i].type == identsym)
	{
		symidx = findsymbol(list[i].name, 2);
		i++;
		

		 //error 6 + 19 check
		if (symidx == -1)
		{
			// error check with kind 1
			symidx = findsymbol(list[i].name, 1);
			if (symidx == -1)
			{
				// error check with kind 3
				symidx = findsymbol(list[i].name, 3);
				if (symidx == -1)
				{
					printparseerror(19);
					exit(0);
				}
				else
				{
					printparseerror(6);
					exit(0);
				}
			}
			else
			{
				printparseerror(6);
				exit(0);
			}
		}

		token_type arith = list[i].type; // HOMEWORK 4 EDIT
		// check becomessym
		if (list[i].type == becomessym)
		{
			i++;
			expression(list);
			emit(4, level - table[symidx].level, table[symidx].addr);
		}

		// HOMEWORK 4 CHANGES START
		else if (list[i].type == addassignsym || list[i].type == subassignsym || list[i].type == timesassignsym || list[i].type == divassignsym || list[i].type == modassignsym)
		{
			i++;
			emit(3, level - table[symidx].level, table[symidx].addr);
			expression(list);

			if (arith == addassignsym)
				emit(2, 0, 2);
			else if (arith == subassignsym)
				emit(2, 0, 3);
			else if (arith == timesassignsym)
				emit(2, 0, 4);
			else if (arith == divassignsym)
				emit(2, 0, 5);
			else if (arith == modassignsym)
				emit(2, 0, 6);

			emit(4, level - table[symidx].level, table[symidx].addr);
		}
		// HOMEWORK 4 CHANGES END
		
		// assign missing error
		else
		{
			printparseerror(5);
			exit(0);
		}
	}


	// CALL STATEMENT

	else if (list[i].type == callsym)
	{
		i++;
		symidx = findsymbol(list[i].name, 3);
		if (list[i].type == identsym)
		{

			// error 7 + 19 check
			if (symidx == -1)
			{
				// check if const found (error 7)
				symidx = findsymbol(list[i].name, 1);
				if (symidx == -1)
				{
					// check if var found (error 7)
					symidx = findsymbol(list[i].name, 2);
					if(symidx == -1)
					{
						// undecl ident (error 19)
						if (symidx == -1)
						{
							printparseerror(19);
							exit(0);
						}
						else
						{
							printparseerror(7);
							exit(0);
						}
					}
					else
					{
						printparseerror(7);
						exit(0);
					}
				}
				else
				{
					printparseerror(7);
					exit(0);
				}
			}
			emit(5, level - table[symidx].level, symidx);
			i++;
		}
		else
		{
			printparseerror(7);
			exit(0);
		}
	}


	// BEGIN STATEMENT

	else if (list[i].type == beginsym)
	{
		// do while loop while semicolonsym
		do
		{
			i++;
			statement(list);
		} while (list[i].type == semicolonsym);

		//continues if there is endsym at end
		if (list[i].type != endsym)
		{
			// check for no semicolon error
			if (list[i].type == identsym || list[i].type == readsym || list[i].type == writesym || list[i].type == beginsym || list[i].type == callsym || list[i].type == ifsym || list[i].type == whilesym)
			{
				printparseerror(15);
				exit(0);
			}
			else if (list[i].type != identsym && list[i].type != readsym && list[i].type != writesym && list[i].type != beginsym && list[i].type != callsym && list[i].type != ifsym && list[i].type != whilesym)
			{
				printparseerror(16);
				exit(0);
			}
		}
		i++;
	}


	// IF STATEMENT

	else if (list[i].type == ifsym)
	{
		i++;
		condition(list);
		if (list[i].type == thensym)
		{
			i++;
		}
		//error 8 check
		else
		{
			printparseerror(8);
			exit(0);
		}

		jpcidx = cIndex;
		emit(8, level, 0);
		statement(list);

		// next token + check elsesym
		//i++;
		if (list[i].type == elsesym)
		{
			i++;
			jmpidx = cIndex;
			emit(7, level, 0);
			code[jpcidx].m = cIndex * 3;
			statement(list);
			code[jmpidx].m = cIndex * 3;
		}
		else
		{
			code[jpcidx].m = cIndex * 3;
		}
	}


	// WHILE STATEMENT - may need i++?

	else if (list[i].type == whilesym)
	{
		loopidx = cIndex;
		i++;
		condition(list);
		// no do error check
		if (list[i].type != dosym)
		{
			printparseerror(9);
			exit(0);
		}
		//else
		i++;
		jpcidx = cIndex;
		emit(8, level, 0);
		statement(list);
		emit(7, level, loopidx * 3);
		code[jpcidx].m = cIndex * 3;
	}

	// READ STATEMENT

	else if (list[i].type == readsym)
	{
		i++;
		if (list[i].type == identsym)
		{
			symidx = findsymbol(list[i].name, 2);
			i++;
			
			if (symidx == -1)
			{
				// error check with kind 1
				symidx = findsymbol(list[i].name, 1);
				if (symidx == -1)
				{
					// error check with kind 3
					symidx = findsymbol(list[i].name, 3);
					if (symidx == -1)
					{
						printparseerror(19);
						exit(0);
					}
					else
					{
						printparseerror(6);
						exit(0);
					}
				}
				else
				{
					printparseerror(6);
					exit(0);
				}
			}
			emit(9, 0, 2);
			emit(4, level - table[symidx].level, table[symidx].addr);
		}
		else
		{
			printparseerror(6);
			exit(0);
		}
	}


	// WRITE STATEMENT

	else if (list[i].type == writesym)
	{
		i++;
		expression(list);
		emit(9, 0, 1);
	}

	else
		return;
}


// conditon function
void condition(lexeme *list)
{
	expression(list);
	//i++;
	if (list[i].type == eqlsym)
	{
		i++;
		expression(list);
		emit(2, level, 7);
	}

	else if (list[i].type == neqsym)
	{
		i++;
		expression(list);
		emit(2, level, 8);
	}

	else if (list[i].type == lessym)
	{
		i++;
		expression(list);
		emit(2, level, 9);
	}

	else if (list[i].type == leqsym)
	{
		i++;
		expression(list);
		emit(2, level, 10);
	}

	else if (list[i].type == gtrsym)
	{
		i++;
		expression(list);
		emit(2, level, 11);
	}

	else if (list[i].type == geqsym)
	{
		i++;
		expression(list);
		emit(2, level, 12);
	}
	else
	{
		printparseerror(10);
		exit(0);
	}
}

void expression(lexeme *list)
{
	if (list[i].type == minussym)
	{
		i++;
		term(list);
		emit(2, 0, 1);
	}
	else
		term(list);


	while (list[i].type == plussym || list[i].type == minussym)
	{
		if (list[i].type == plussym)
		{
			i++;
			term(list);
			emit(2, 0, 2);
		}

		else
		{
			i++;
			term(list);
			emit(2, 0, 3);
		}
	}

}

void term(lexeme *list)
{
	factor(list);
	while (list[i].type == multsym || list[i].type == slashsym || list[i].type == modsym)
	{
		if (list[i].type == multsym)
		{
			i++;
			factor(list);
			emit(2, level, 4);
		}

		else if (list[i].type == slashsym)
		{
			i++;
			factor(list);
			emit(2, level, 5);
		}

		else
		{
			i++;
			factor(list);
			emit(2, level, 6);
		}
	}
}

void factor(lexeme *list)
{
	int var;
	int constv;
	int proc;
	// check token for ident
	if (list[i].type == identsym)
	{
		// check for variable, constant or procedure
		var = findsymbol(list[i].name, 2);
		constv = findsymbol(list[i].name, 1);
		proc = findsymbol(list[i].name, 3);

		// variable but not constant
		if (var != -1 && constv == -1)
		{
			emit(3, level-table[var].level, table[var].addr);
		}
		// constant but not variable ****CHANGED THIS LEVEL
		else if (var == -1 && constv != -1)
		{
			emit(1, level-table[constv].level, table[constv].value);
		}
		// variable and constant possible
		else
		{
			// level check to choose
			if (table[var].level > table[constv].level)
			{
				emit(3, level - table[var].level, table[var].addr);
			}
			else //****CHANGED THIS LEVEL
			{
				emit(1, level - table[constv].level, table[constv].value);
			}
		}
		// check if procedure
		if (proc != -1)
		{
			printparseerror(11);
			exit(0);
		}
		// if neither variable constant nor procedure
		if (var == -1 && constv == -1 && proc == -1)
		{
			printparseerror(19);
			exit(0);
		}
		i++;
	}

	// check if token is number *** CHANGED THIS LEVEL
	else if (list[i].type == numbersym)
	{
		emit(1, level - table[symidx].level, list[i].value);
		i++;
	}

	// check if token is left parenth.
	else if (list[i].type == lparentsym)
	{
		i++;
		expression(list);
		if (list[i].type != rparentsym)
		{
			printparseerror(12);
			exit(0);
		}
		i++;
	}

	 //error 11 check
	else
	{
		printparseerror(11);
		exit(0);
	}
}