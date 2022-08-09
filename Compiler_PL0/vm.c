/* Ishmael Garcia
   HW4 - VIRTUAL MACHINE
   COP3402 - SUMMER 2022
   DR.MONTAGNE */

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

// constants
#define MAX_PAS_LENGTH 500

// variables
int SP, BP, PC = 0;
int IR[3];					// went with array for IR instead of struct
int PAS[MAX_PAS_LENGTH];
int readFile = 0;
int halt = 1;
int pasIndex, initBP = 0;

// base function
int base(int L)
{
	int arb = BP;

	while (L > 0)
	{
		arb = PAS[arb];
		L--;
	}
	return arb;
}

void virtual_machine(instruction *code)
{
	int i = 0;
	// arrays of opcodes 
	char opCode[9][4] = { "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SYS" };
	char oprCode[13][4] = { "RTN", "NEG", "ADD", "SUB", "MUL", "DIV", "MOD", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ" };
	

	// initial process address space values all zero
	for (int p = 0; p < 500; p++)
	{
		PAS[p] = 0;
	}

	// changes for hw4 start does not work
	//int z = 0;
	//int k = 0;
	while (code[i].op != -1)
	{
		PAS[BP] = code[i].op;
		PAS[BP + 1] = code[i].l;
		PAS[BP + 2] = code[i].m;
		BP += 3;
		i++;
	}

	// changes for hw4 end

	// initial values once program uploaded to PAS
	SP = BP - 1;
	//BP = SP + 1;
	PC = 0;

	// Set pasIndex and initBP for print (need initial base pointer for stack)
	pasIndex = BP;
	initBP = BP;

	// Print initial values
	printf("                PC\tBP\tSP\tstack");
	printf("\nInitial Values:	%d\t%d\t%d\n", PC,BP,SP );
	
	while (halt != 0)
	{
		// FETCH CYCLE 
		IR[0] = PAS[PC];		// OPcode
		IR[1] = PAS[PC + 1];	// L(lex level)
		IR[2] = PAS[PC + 2];	// M value

		PC = PC + 3;
		// FETCH CYCLE END

		// EXECUTE CYCLE
		switch (IR[0])
		{
			case 1:				// LIT
				SP += 1;
				PAS[SP] = IR[2];
				break;
			case 2:				// OPR
				switch (IR[2])
				{
					case 0:		// OPR - RTN
						SP = BP - 1;
						BP = PAS[SP + 2];
						PC = PAS[SP + 3];
						break;
					case 1:		// OPR - NEG
						PAS[SP] = -1 * PAS[SP];
						break;
					case 2:		// OPR - ADD
						SP = SP - 1;
						PAS[SP] = PAS[SP] + PAS[SP + 1];
						break;
					case 3:		// OPR - SUB
						SP = SP - 1;
						PAS[SP] = PAS[SP] - PAS[SP + 1];
						break;
					case 4:		// OPR - MUL
						SP = SP - 1;
						PAS[SP] = PAS[SP] * PAS[SP + 1];
						break;
					case 5:		// OPR - DIV
						SP = SP - 1;
						PAS[SP] = PAS[SP] / PAS[SP + 1];
						break;
					case 6:		// OPR - MOD
						SP = SP - 1;
						PAS[SP] = PAS[SP] % PAS[SP + 1];
						break;
					case 7:		// OPR - EQL
						SP = SP - 1;
						PAS[SP] = PAS[SP] == PAS[SP + 1];
						break;
					case 8:		// OPR - NEQ
						SP = SP - 1;
						PAS[SP] = PAS[SP] != PAS[SP + 1];
						break;
					case 9:		// OPR - LSS
						SP = SP - 1;
						PAS[SP] = PAS[SP] < PAS[SP + 1];
						break;
					case 10:	// OPR - LEQ
						SP = SP - 1;
						PAS[SP] = PAS[SP] <= PAS[SP + 1];
						break;
					case 11:	// OPR - GTR
						SP = SP - 1;
						PAS[SP] = PAS[SP] > PAS[SP + 1];
						break;
					case 12:	// OPR - GEQ
						SP = SP - 1;
						PAS[SP] = PAS[SP] >= PAS[SP + 1];
						break;
				}
				break;
			case 3:				// LOD
				SP = SP + 1;
				PAS[SP] = PAS[base(IR[1]) + IR[2]];
				break;
			case 4:				// STO
				PAS[base(IR[1]) + IR[2]] = PAS[SP];
				SP = SP - 1;
				break;
			case 5:				// CAL
				PAS[SP + 1] = base(IR[1]);	// **static link (SL)
				PAS[SP + 2] = BP;			// **dynamic link (DL)
				PAS[SP + 3] = PC;			// **return address (RA)
				BP = SP + 1;
				PC = IR[2];
				break;
			case 6:				// INC
				SP = SP + IR[2];
				break;
			case 7:				// JMP
				PC = IR[2];
				break;
			case 8:				// JPC
				if (PAS[SP] == 0)
				{
					PC = IR[2];
				}
				SP = SP - 1;
				break;
			case 9:				// SYS
				switch (IR[2])
				{
					case 1:		// SYS - WRITE
						printf("Output result is: %d\n", PAS[SP]);
						SP = SP - 1;
						break;
					case 2:		// SYS - READ
						SP = SP + 1;
						printf("Please Enter an Integer: ");
						scanf("%d", &readFile);
						PAS[SP] = readFile;
						break;
					case 3:		// SYS - HALT
						halt = 0;
						break;
					default:
						printf("SYS COMMAND ERROR");
				}
				break;
			default:
				printf("MAIN SWITCH ERROR");
				break;
		}
		// EXECUTE CYCLE END


		// Print after EXECUTE CYCLE
		switch (IR[0])
		{
			// if OPR print based on M value
			case 2:
				printf("%s\t%d  %d\t%d\t%d\t%d\t", oprCode[IR[2]], IR[1], IR[2], PC, BP, SP);
				break;
			// else print opcode
			default:
				printf("%s\t%d  %d\t%d\t%d\t%d\t",opCode[IR[0] - 1], IR[1], IR[2], PC, BP, SP);
				break;
		}

		// after execute set PAS Index to initial base pointer
		pasIndex = initBP;
		// loop until PAS index reaches stack pointer
		while (pasIndex <= SP)
		{
			// add "|" if BP not pointing to initial frame
			if (pasIndex == BP && BP != initBP)
			{
				printf("|");
			}	
			// stack is done, dont print
			if (pasIndex > SP)	
			{
				break;
			}
			// increment and output stack
			printf("%d ", PAS[pasIndex++]);

		}// end of output while
		// single empty line at end
		printf("\n");
	}
}