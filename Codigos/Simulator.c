#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int callLoader();
void showMemory();
void showBankRegisters();
void cleanMemory();
void cleanRegisters();
void executeProgram();
void callControlUnit();
int getBinaryRange(int nBits, char signal);
void writeFile();
//------------------ULA OPERATIONS-----------------------------------
int ula_add(int op1, int op2);
unsigned int ula_addu(unsigned int op1, unsigned int op2);
int ula_clz(int op);
int ula_clo(int op);
int ula_sub (int op1, int op2);
unsigned subu (unsigned int op1, unsigned int op2);
int ula_seh(int op);
int ula_seb(int op);
int ula_and(int op1, int op2);
int ula_nor(int op1, int op2);
int ula_or(int op1, int op2);
int ula_xor(int op1, int op2);
int ula_div(int op1, int op2);
unsigned int ula_divu(unsigned int op1, unsigned int op2);
int ula_mult(int op1, int op2);
unsigned int ula_multu(unsigned int op1, unsigned int op2);
int ula_sllv(int op1, int op2);
int ula_srlv(int op1, int op2);
int ula_srav(int op1, int op2);
int ula_rotrv(int op1, int op2);
int ula_slt(int op1, int op2);
unsigned int ula_sltu(unsigned int op1, unsigned int op2);
int ula_equal(int op1, int op2);
//------------------ULA OPERATIONS-----------------------------------


//------------------BRANCHS OPERATIONS-------------------------------
void beq(int op1, int op2, int offset);
//------------------BRANCHS OPERATIONS-------------------------------


//------------------LOAD/STORE OPERATIONS----------------------------
void sw(int value, int offset, int baseRegister);
//------------------LOAD/STORE OPERATIONS----------------------------


//------------------JUMP OPERATIONS-------------------------------
void j(int address);
//------------------JUMP OPERATIONS-------------------------------


//The processor memory(64KB) with 16384 32-bits address 
int memory[16384];
//General purpose registers
int GPR[32];
//The next memory address to program
unsigned int next_free_address = 0;
//Program Counter register
unsigned int PC = 0;
//Instruction register
unsigned int IR = 0;
//Accumulator registers
int HI = 0;
int LO = 0;
//Flags: 0 - zero(z), 1 - signal(s), 2 - carry(c), 3 - overflow(o), 4 - parity(p), 
int flags[5];

void main()
{

    int success;
    while(1){
        printf("TEC499 - SIMULATOR\n\n");
        //Reads the file and load the memory
        success = callLoader();

        //Ok
        if(success == 1){
            showMemory();
            printf("\n*Successfully read program\n");
            executeProgram();
            showMemory();
            showBankRegisters();
        }
        //Error
        else
        	printf("\n*Error on read program\n");
        cleanMemory();
        cleanRegisters();
        printf("\n\n*Press any button to read other program");
        getch();
        system("cls");
    }

}

//Read the binary file and load the program on memory
int callLoader()
{
    //Takes the instructions
    int instruction = 0;
    int i, numberOfInstructions = 0;
    char c;

    printf("-> Loading program on binary.txt\n\n");
    FILE *file = fopen("binary.txt", "r");

    //If the file is correct
    if(file == NULL)
        return 0;
	
    //Reads all file
    while(!feof(file))
    {
        instruction = 0;
        c = fgetc(file);

        //Read all 32 bits in instruction
        for(i = 0; i < 32; i++)
        {
            //Shows the current bit
            printf("%c",c);

            //If is '1' increment +1
            if (c == '1')
                instruction++;
            //If is not the last bit, shift the read bit to left
            if (i != 31)
                instruction = (instruction << 1);

            //Reads '\n'
            c = fgetc(file);
        }
        //Checks the number of instructions on program
        if(numberOfInstructions == 0){
        	GPR[28] = instruction;
        	numberOfInstructions++;
		}
        	
		else{
			//Put the instruction on memory and increments the next memory address
        	memory[next_free_address] = instruction;
        	next_free_address++;	
		}
        printf("\n");
    }
    
    //Defines the $sp and $fp address
    GPR[29] = next_free_address;
    GPR[30] = GPR[29];    
	fclose(file);
    return 1;
}

//Shows all values in memory and your addresses
void showMemory()
{
    int i = 0;
    float memoryUsage = (GPR[29]*4);
    printf("\nUSED MEMORY:\t%.4fKB\n---------------------------------------------------------------------------\n\tAddress\t\tValue\n", memoryUsage/1024);
    for(i; i <= next_free_address; i++){
    	if(GPR[28] == i)
    		printf("$gp ->");
		if(PC == i && GPR[28] != PC)
    		printf("PC ->");
    	if(GPR[30] == i && GPR[28] != GPR[30])
    		printf("$fp ->");
    	if(GPR[29] == i && GPR[30] != GPR[29])
    		printf("$sp ->");
    	printf("\t0x%04x\t\t%d\n", i, memory[i]);
	}
	printf("---------------------------------------------------------------------------\n");
}

//Shows all registers and your values
void showBankRegisters()
{
	int i = 0;
	printf("\nBANK REGISTERS\n-----------------------------------------------------------------------------\n");
    for(i; i < 32; i++)
    	printf("$%d = %d\n", i, GPR[i]);
    printf("\nAccumulator: HI = %d | LO = %d\n", HI, LO);
	printf("-----------------------------------------------------------------------------\n");
}

//Cleans the memory
void cleanMemory()
{
    int i = 0;
    for(i; i < next_free_address; i++)
        memory[i] = 0;
    next_free_address = 0;
}

//Cleans all processor registers
void cleanRegisters()
{
	int i = 0;
	for(i; i < 32; i++)
		GPR[i] = 0;
	PC = 0;
	IR = 0;
	HI = 0;
	LO = 0;
}

//Execute the program loaded on memory
void executeProgram()
{
	printf("\n-----------------------------EXECUTING PROGRAM-----------------------------\n");
	//Executes the program while PC address is a instruction address on memory
	while(PC != GPR[28]){
		//Takes the memory value and puts on IR
		IR = memory[PC];
		//Calls the control unit
		callControlUnit();
	}
	printf("-----------------------------------------------------------------------------\n");
}

//Calls the control unit to start the instruction execution
void callControlUnit()
{
	printf("   PC -> 0x%04x | IR = %d\n\tInstruction: ", PC, IR);
	//Shift the IR to find the opcode[31...26]
	unsigned int opcode = IR >> 26;
	
	//R instruction
	if(opcode == 0 || opcode  == 28 || opcode  == 31){
		//Find the R instruction function [5...0]
		unsigned int function = IR & 63;
		//Find the R instruction shamt [10...6]
		unsigned int shamt = (IR >> 6) & 31;
		//Find the R instruction destiny register [15...11]
		unsigned int rd = (IR >> 11) & 31;
		//Find the R instruction operand register 2 [20...16]
		unsigned int rt = (IR >> 16) & 31;
		//Find the R instruction operand register 1[25...21]
		unsigned int rs = (IR >> 21) & 31;
		
		//Type special
		if(opcode == 0){
			//Calls the specific instruction with your parameters
			switch(function){
				case 0:
					printf("sll\n\t\tOperation: ");
					break;
				case 2:
					if(rs == 0){
						printf("srl\n\t\tOperation: ");
					}
					else if(rs == 1){
						printf("rotr\n\t\tOperation: ");
					}
					break;
				case 3:
					printf("sra\n\t\tOperation: ");
					break;
				case 4:
					printf("sllv\n\t\tOperation: ");
					break;
				case 6:
					if(shamt == 0){
						printf("srlv\n\t\tOperation: ");
					}
					else if(shamt == 1){
						printf("rotrv\n\t\tOperation: ");
					}
					break;
				case 7:
					printf("srav\n\t\tOperation: ");
					break;
				case 8:
					printf("jr\n\t\tOperation: ");
					break;
				case 9:
					printf("jalr\n\t\tOperation: ");
					break;
				case 10:
					printf("movz\n\t\tOperation: ");
					break;
				case 11:
					printf("movn\n\t\tOperation: ");
					break;
				case 16:
					printf("mfhi\n\t\tOperation: ");
					break;
				case 17:
					printf("mthi\n\t\tOperation: ");
					break;
				case 18:
					printf("mflo\n\t\tOperation: ");
					break;
				case 19:
					printf("mtlo\n\t\tOperation: ");
					break;
				case 24:
					printf("mult\n\t\tOperation: ");
					break;
				case 25:
					printf("multu\n\t\tOperation: ");
					break;	
				case 26:
					printf("div\n\t\tOperation: ");
					break;
				case 27:
					printf("divu\n\t\tOperation: ");
					break;
				case 32:
					printf("add\n\t\tOperation: $%d = $%d + $%d ", rd, rs, rt);
					GPR[rd] = ula_add(GPR[rs], GPR[rt]);
					PC++;
					break;	
				case 33:
					printf("addu\n\t\tOperation: ");
					break;
				case 34:
					printf("sub\n\t\tOperation: ");
					break;
				case 35:
					printf("subu\n\t\tOperation: ");
					break;
				case 36:
					printf("and\n\t\tOperation: ");
					break;
				case 37:
					printf("or\n\t\tOperation: ");
					break;
				case 38:
					printf("xor\n\t\tOperation: ");
					break;
				case 39:
					printf("nor\n\t\tOperation: ");
					break;
				case 42:
					printf("slt\n\t\tOperation: ");
					break;
				case 43:
					printf("sltu\n\t\tOperation: ");
					break;
			}	
		}
		
		//Type special
		else if(opcode == 28){
			//Calls the specific instruction with your parameters
			switch(function){
				case 0:
					printf("madd\n\t\tOperation: ");
					break;
				case 1:
					printf("maddu\n\t\tOperation: ");
					break;
				case 2:
					printf("mul\n\t\tOperation: $%d = $%d * $%d", rd, rs, rt);
					GPR[rd] = ula_mult(GPR[rs], GPR[rt]);
					PC++;
					break;
				case 4:
					printf("msub\n\t\tOperation: ");
					break;
				case 5:
					printf("msubu\n\t\tOperation: ");
					break;
				case 32:
					printf("clz\n\t\tOperation: ");
					break;
				case 33:
					printf("clo\n\t\tOperation: ");
					break;
			}	
		}
		
		//Type special
		else if(opcode == 31){
			//Calls the specific instruction with your parameters
			switch(function){
				case 0:
					printf("ext\n\t\tOperation: ");
					break;
				case 4:
					printf("ins\n\t\tOperation: ");
					break;
				case 32:
					if(shamt == 16){
						printf("seb\n\t\tOperation: ");
					}
					else if(shamt == 24){
						printf("seh\n\t\tOperation: ");
					}
					else if(shamt == 2){
						printf("wsbh\n\t\tOperation: ");
					}
					break;
			}	
		}
	}
	//I instruction
	else if(opcode == 1 || (opcode >= 4 && opcode <= 27) || opcode == 29 || opcode == 30 || opcode > 31){
		//Find the I instruction immediate [15...0]
		short int immediate = IR & 65535;
		//Find the I instruction destiny register [20...16]
		unsigned int rt = (IR >> 16) & 31;
		//Find the I instruction operand register [25...21]
		unsigned int rs = (IR >> 21) & 31;
		
		//Calls the specific instruction with your parameters
		switch(opcode){
			case 1:
				printf("bltz\n\t\tOperation: ");
				break;
			case 4:
				printf("beq\n\t\tOperation: $%d == $%d -> ", rs, rt);
				beq(GPR[rs], GPR[rt], immediate);
				break;
			case 5:
				printf("bne\n\t\tOperation: ");
				break;
			case 7:
				printf("bgtz\n\t\tOperation: ");
				break;
			case 8:
				printf("addi\n\t\tOperation: $%d = $%d + %d ", rt, rs, immediate);
				GPR[rt] = ula_add(GPR[rs], immediate);
				PC++;
				break;
			case 9:
				printf("addiu\n\t\tOperation: ");
				break;
			case 10:
				printf("slti\n\t\tOperation: ");
				break;
			case 11:
				printf("sltiu\n\t\tOperation: ");
				break;
			case 12:
				printf("andi\n\t\tOperation: ");
				break;
			case 13:
				printf("ori\n\t\tOperation: ");
				break;
			case 14:
				printf("xori\n\t\tOperation: ");
				break;
			case 15:
				printf("lui\n\t\tOperation: ");
				break;
			case 32:
				printf("lb\n\t\tOperation: ");
				break;
			case 33:
				printf("lh\n\t\tOperation: ");
				break;
			case 35:
				printf("lw\n\t\tOperation: ");
				break;
			case 40:
				printf("sb\n\t\tOperation: ");
				break;
			case 41:
				printf("sh\n\t\tOperation: ");
				break;
			case 43:
				printf("sw\n\t\tOperation: $%d -> %d($%d)", rt, immediate, rs);
				sw(GPR[rt], immediate, rs);
				PC++;
				break;
		}
	}
	//J instruction
	else if(opcode == 2 || opcode == 3){
		//Find the J instruction address [25...0]
		unsigned int address = IR & 67108863;

		//Calls the specific instruction with your parameters
		switch(opcode){
			case 2:
				printf("j\n\t\tOperation: PC = 0x%04x", address);
				j(address);
				break;
			case 3:
				printf("jal\n\t\tOperation: ");
				break;
		}
	}
	printf("\n\n");
}

//Takes the binary range
int getBinaryRange(int nBits, char signal)
{
	nBits--;
	if(signal == '+')
		return pow(2,nBits) -1;
	else if(signal == '-')
		return pow(2,nBits)*(-1);
	return 0;
	
}

//The sum logic in ULA
int ula_add(int op1, int op2)
{
	int sum = op1 + op2;
	printf("= %d + %d = %d", op1, op2, sum);
	//Check flag conditions
	if(sum == 0){
		flags[0] = 1;
		printf(" (Flag zero activated)");
	}
	else if(sum > getBinaryRange(32, '+') || sum < getBinaryRange(32, '-')){
		flags[3] = 1;
		printf(" (Flag overflow activated)");
	}
	return sum;
}

//The multiplication logic in ULA
int ula_mult(int op1, int op2)
{
	int mult = op1 * op2;
	printf(" = %d * %d = %d", op1, op2, mult);
	//Check flag conditions
	if(mult == 0){
		flags[0] = 1;
		printf(" (Flag zero activated)");
	}
	else if(mult > getBinaryRange(32, '+') || mult < getBinaryRange(32, '-')){
		flags[3] = 1;
		printf(" (Flag overflow activated)");
	}
	return mult;
}

//The ULA equal comparation
int ula_equal(int op1, int op2)
{
	int comparation = 0;
	if(op1 == op2)
		comparation = 1;
		
	if(comparation == 0){
		flags[0] = 1;
		printf("No (Flag zero activated)");
	}
	else
		printf("Yes");
	return comparation;
}

//The beq instruction implementation
void beq(int op1, int op2, int offset)
{
	printf("%d == %d? ", op1, op2);
	int comparation = ula_equal(op1, op2);
	if(comparation == 1){
		printf(" -> PC = 0x%04x + (%d)",PC,offset);
		PC = PC + offset;
	}
	else
		PC++;
}

//The j instruction implementation
void j(int address)
{
	PC = address;
}

void sw(int value, int offset, int baseRegister)
{
	int address = GPR[baseRegister];
	printf(" = %d -> (0x%04x + %d)", value, address, offset);
	memory[address + offset] = value;
	
	if(baseRegister == 29){
		next_free_address++;
		int i;
		printf("\n\t\t\tSTACK\n");
		for(i = next_free_address; i >= GPR[28]; i--)
			printf("\t\t\t0x%04x -> %d\n", i, memory[i]);
	}
}


