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
int ula_clzclo(int op, int op2);
int ula_sub (int op1, int op2);
unsigned ula_subu (unsigned int op1, unsigned int op2);
void lui(int op1, int op2);
void ula_seh(int op1, int op2);
void ula_seb(int op1, int op2);
int ula_and(int op1, int op2);
int ula_nor(int op1, int op2);
int ula_or(int op1, int op2);
int ula_xor(int op1, int op2);
int ula_div(int op1, int op2);
unsigned int ula_divu(unsigned int op1, unsigned int op2);
void ula_mult(int op1, int op2);
unsigned int ula_multu(unsigned int op1, unsigned int op2);
int ula_sllv(int op1, int op2);
int ula_srlv(int op1, int op2);
int ula_srav(int op1, int op2);
int ula_rotrv(int op1, int op2);
int ula_slt(int op1, int op2);
unsigned int ula_sltu(unsigned int op1, unsigned int op2);
int ula_equal(int op1, int op2);
//------------------ULA OPERATIONS-----------------------------------
void madd(int op1, int op2);
void maddu(unsigned int op1, unsigned int op2);
void msub(int op1, int op2);
void msubu(unsigned int op1, unsigned int op2);
int rotr(int op2, int offset);
int rotrv(int op2, int op3);
//------------------ACCUMULATOR-----------------------------------
int movefromHILO(int op);
void movetoHILO(int value, int op);
//------------------ACCUMULATOR-----------------------------------
void bgtz(int op1, int offset);
void bltz(int op1, int offset);
//------------------BRANCHS OPERATIONS-------------------------------
void beq(int op1, int op2, int offset);
void bne(int op1, int op2, int offset);
//------------------BRANCHS OPERATIONS-------------------------------


//------------------LOAD/STORE OPERATIONS----------------------------
void sw(int value, int offset, int baseRegister);
void sh(int value, int offset, int baseRegister);
void sb(int value, int offset, int baseRegister);
void lw(int regist, int offset, int baseRegister);
void lh(int regist, int offset, int baseRegister);
void lb(int regist, int offset, int baseRegister);
//------------------LOAD/STORE OPERATIONS----------------------------


//------------------JUMP OPERATIONS-------------------------------
void j(int address);
void jal(int address);
void jr(int regist);
void jalr(int rd, int rs);
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

FILE *file;
int wordsQuantity = 0;

void main()
{
    int success;
    while(1){
        printf("TEC499 - SIMULATOR\n\n");
        //Reads the file and load the memory
        success = callLoader();

        //Ok
        if(success == 1){
            printf("\n*Program read and executed\n");
            file = fopen("log.txt", "w");
            executeProgram();
            fclose(file);
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
			wordsQuantity++;	
		}
        printf("\n");
    }
    
    //Defines the $sp and $fp address
    GPR[30] = 16383;
    GPR[29] = GPR[30] + 1;
    next_free_address--;
	fclose(file);
    return 1;
}

//Shows all values in memory and your addresses
void showMemory()
{
    int i = 0;
    float memoryUsage = (wordsQuantity*4);
    printf("\nUSED MEMORY:\t%.4fKB\n---------------------------------------------------------------------------\n\tAddress\t\tValue\n", memoryUsage/1024);
    for(i; i <= next_free_address; i++){
    	if(GPR[28] == i)
    		printf("$gp ->");
		if(PC == i && GPR[28] != PC)
    		printf("PC ->");
    	if(GPR[30] == i && GPR[28] != GPR[30])
    		printf("$fp ->");
    	if(GPR[29] == i && GPR[30] != GPR[29] && GPR[29] > GPR[30])
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
	fputs("----------------------------------PROGRAM LOG----------------------------------\n\n", file);
	//Executes the program while PC address is a instruction address on memory
	while(PC != GPR[28]){
		//Takes the memory value and puts on IR
		IR = memory[PC];
		PC++;
		//Calls the control unit
		callControlUnit();
	}
	fputs("--------------------------------------------------------------------------------\n", file);
}

//Calls the control unit to start the instruction execution
void callControlUnit()
{
	char str[100];
	sprintf(str,"   PC -> 0x%04x | IR = %d\n\tInstruction: ", PC-1, IR);
	fputs(str, file);
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
					sprintf(str,"sll\n\t\tOperation: $%d = $%d << %d", rd, rt, shamt); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_sllv(GPR[rt], shamt);
					break;
				case 2:
					if(rs == 0){
						sprintf(str,"srl\n\t\tOperation: $%d = $%d >> %d", rd, rt, shamt); //-------------------------------------TESTED
						fputs(str, file);
						GPR[rd] = ula_srlv(GPR[rt], shamt);
					}
					else if(rs == 1){
						sprintf(str,"rotr\n\t\tOperation:  %d <<  %d x (right) %d", rd, rt, shamt); //------------------------------------- TESTED
						fputs(str, file);
						GPR[rd] = rotr(GPR[rt], shamt);
					}
					break;
				case 3:
					sprintf(str,"sra\n\t\tOperation: $%d = $%d >> %d", rd, rt, shamt);
					fputs(str, file);
					GPR[rd] = ula_srlv(GPR[rt], shamt);
					break;
				case 4:
					sprintf(str,"sllv\n\t\tOperation: $%d = $%d << $%d", rd, rt, rs); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_sllv(GPR[rt], GPR[rs]);
					break;
				case 6:
					if(shamt == 0){
						sprintf(str,"srlv\n\t\tOperation:  $%d = $%d >> $%d", rd, rt, rs); //-------------------------------------TESTED
						fputs(str, file);
						GPR[rd] = ula_srlv(GPR[rt], GPR[rs]);
					}
					else if(shamt == 1){
						sprintf(str,"rotrv\n\t\tOperation:%d <<  %d x (right) %d", rd, rt, rs); //------------------------------------- TESTED
						fputs(str, file);
						GPR[rd] = rotrv(GPR[rt], GPR[rs]);
					}
					break;
				case 7:
					sprintf(str,"srav\n\t\tOperation: $%d = $%d >> $%d", rd, rt, rs); //------------------------------------- TESTED
					fputs(str, file);
					GPR[rd] = ula_srlv(GPR[rt], GPR[rs]);
					break;
				case 8:
					sprintf(str,"jr\n\t\tOperation: PC = 0x%04x", GPR[31]); //-------------------------------------TESTED
					fputs(str, file);
					jr(rs);
					break;
				case 9:
					sprintf(str,"jalr\n\t\tOperation: $%d = 0x%04x and PC = $%d = 0x%04x", rd, PC, rs, GPR[rs]); //-------------------------------------TESTED
					fputs(str, file);
					jalr(rd, rs);
					break;
				case 10:
					sprintf(str,"movz\n\t\tOperation: Se $%d = 0 -> $%d = $%d = %d", rt, rd, rs, GPR[rs]); //-------------------------------------TESTED
					fputs(str, file);
					int comp1 = ula_equal(rt, 0);
					if (comp1 == 1){
						GPR[rd] = GPR[rs];
					}
					break;
				case 11:
					sprintf(str,"movn\n\t\tOperation: Se $%d != 0 -> $%d = $%d = %d", rt, rd, rs, GPR[rs]); //-------------------------------------TESTED
					fputs(str, file);
					int comp2 = ula_equal(rt, 0);
					if (comp2 == 0){
						GPR[rd] = GPR[rs];
					}
					break;
				case 16:
					sprintf(str,"mfhi\n\t\tOperation: $%d = HI", rd); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = movefromHILO(1);
					break;
				case 17:
					sprintf(str,"mthi\n\t\tOperation: HI = $%d", rs); //-------------------------------------TESTED
					fputs(str, file);
					movetoHILO(GPR[rs], 1);
					break;
				case 18:
					sprintf(str,"mflo\n\t\tOperation: $%d = LO", rd); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = movefromHILO(0);
					break;
				case 19:
					sprintf(str,"mtlo\n\t\tOperation: LO = $%d", rs); //-------------------------------------TESTED
					fputs(str, file);
					movetoHILO(GPR[rs], 0);
					break;
				case 24:
					sprintf(str,"mult\n\t\tOperation: HI e LO = $%d * $%d =", rs, rt); //-------------------------------------TESTED
					fputs(str, file);
					ula_mult(GPR[rs], GPR[rt]);
					break;
				case 25:
					sprintf(str,"multu\n\t\tOperation: HI e LO = $%d * $%d =", rs, rt); //-------------------------------------TESTED
					fputs(str, file);
					ula_multu(GPR[rs], GPR[rt]);
					
					break;	
				case 26:
					sprintf(str,"div\n\t\tOperation: $%d/$%d ", rs, rt);
					fputs(str, file);
					ula_div(GPR[rs], GPR[rt]);
					break;
				case 27:
					sprintf(str,"divu\n\t\tOperation: $%d/$%d ", rs, rt);
					fputs(str, file);
					ula_divu(GPR[rs], GPR[rt]);
					
					break;
				case 32:
					sprintf(str,"add\n\t\tOperation: $%d = $%d + $%d ", rd, rs, rt); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_add(GPR[rs], GPR[rt]);
					break;	
				case 33:
					sprintf(str,"addu\n\t\tOperation: $%d = $%d + $%d ", rd, rs, rt); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_addu(GPR[rs], GPR[rt]);
					break;
				case 34:
					sprintf(str,"sub\n\t\tOperation: $%d = $%d - $%d", rd, rs, rt); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_sub(GPR[rs], GPR[rt]);
					break;
				case 35:
					sprintf(str,"subu\n\t\tOperation: $%d = $%d - $%d", rd, rs, rt); //------------------------------------- TESTED
					fputs(str, file);
					GPR[rd] = ula_subu(GPR[rs], GPR[rt]);
					break;
				case 36:
					sprintf(str,"and\n\t\tOperation: $%d = $%d and $%d", rd, rs, rt); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_and(GPR[rs], GPR[rt]);
					break;
				case 37:
					sprintf(str,"or\n\t\tOperation: $%d = $%d or $%d", rd, rs, rt);  //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_or(GPR[rs], GPR[rt]);
					break;
				case 38:
					sprintf(str,"xor\n\t\tOperation: $%d = $%d xor $%d", rd, rs, rt); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_xor(GPR[rs], GPR[rt]);
					break;
				case 39:
					sprintf(str,"nor\n\t\tOperation: $%d = $%d nor $%d", rd, rs, rt); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_nor(GPR[rs], GPR[rt]);
					break;
				case 42:
					sprintf(str,"slt\n\t\tOperation: $%d < $%d?", rs, rt); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_slt(GPR[rs], GPR[rt]);
					break;
				case 43:
					sprintf(str,"sltu\n\t\tOperation: $%d = $%d < $%d ", rd, rs, rt); //------------------------------------- TESTED 
					fputs(str, file);
					GPR[rd] = ula_sltu(GPR[rs], GPR[rt]);
					break;
			}	
		}
		
		//Type special
		else if(opcode == 28){
			//Calls the specific instruction with your parameters
			switch(function){
				case 0:
					sprintf(str,"madd\n\t\tOperation: HI e LO += $%d * $%d =", rs, rt);
					fputs(str, file);
					madd(GPR[rs],GPR[rt]);
					break;
				case 1:
					sprintf(str,"maddu\n\t\tOperation: HI e LO += $%d * $%d =", rs, rt);
					fputs(str, file);
					madd(GPR[rs],GPR[rt]);
					
					break;
				case 2:
					sprintf(str,"mul\n\t\tOperation: $%d = $%d * $%d", rd, rs, rt);//-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_mul(GPR[rs], GPR[rt]);
					break;
				case 4:
					sprintf(str,"msub\n\t\tOperation: HI e LO -= $%d * $%d =", rs, rt);
					fputs(str, file);
					msub(GPR[rs], GPR[rt]);
					break;
				case 5:
					sprintf(str,"msubu\n\t\tOperation: HI e LO -= $%d * $%d =", rs, rt);
					fputs(str, file);
					msubu(GPR[rs], GPR[rt]);
					break;
				case 32:
					sprintf(str,"clz\n\t\tOperation: $%d = quantidade de 0's de $%d", rd, rs); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_clzclo(GPR[rs], 0); 
					break;
				case 33:
					sprintf(str,"clo\n\t\tOperation: $%d = quantidade de 1's de $%d", rd, rs); //-------------------------------------TESTED
					fputs(str, file);
					GPR[rd] = ula_clzclo(GPR[rs], 1);
					break;
			}	
		}
		
		//Type special
		else if(opcode == 31){
			//Calls the specific instruction with your parameters
			switch(function){
				case 0:
					sprintf(str,"ext\n\t\tOperation: ");
					fputs(str, file);
					break;
				case 4:
					sprintf(str,"ins\n\t\tOperation: ");
					fputs(str, file);
					break;
				case 32:
					if(shamt == 16){
						sprintf(str,"seb\n\t\tOperation: %d = %d << 24", rd, rt);
						fputs(str, file);
						ula_seb(rd, rt);
					}
					else if(shamt == 24){
						sprintf(str,"seh\n\t\tOperation:%d = %d << 16", rd, rt );//-------------------------------------TESTED
						fputs(str, file);
						ula_seh(rd, rt); 
					}
					else if(shamt == 2){
						sprintf(str,"wsbh\n\t\tOperation: ");
						fputs(str, file);
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
				sprintf(str,"bltz\n\t\tOperation: if %d < 0 -> %d ", rs, immediate );
				fputs(str, file);
				bltz(GPR[rs], immediate);
				break;
			case 4:
				sprintf(str,"beq\n\t\tOperation: $%d == $%d -> ", rs, rt, immediate);//-------------------------------------TESTED
				fputs(str, file);
				beq(GPR[rs], GPR[rt], immediate);
				break;
			case 5:
				sprintf(str,"bne\n\t\tOperation: $%d != $%d -> ", rs, rt, immediate);//-------------------------------------TESTED
				fputs(str, file);
				bne(GPR[rs], GPR[rt], immediate);
				break;
			case 7:
				sprintf(str,"bgtz\n\t\tOperation: if %d > 0 -> %d ", rs, immediate ); 
				fputs(str, file);
				bgtz(GPR[rs], immediate);
				break;
			case 8:
				sprintf(str,"addi\n\t\tOperation: $%d = $%d + %d ", rt, rs, immediate); //-------------------------------------TESTED
				fputs(str, file);
				GPR[rt] = ula_add(GPR[rs], immediate);
				break;
			case 9:
				sprintf(str,"addiu\n\t\tOperation: $%d = $%d + %d", rt, rs, immediate); //------------------------------------- TESTED
				fputs(str, file);
				GPR[rt] = ula_addu(GPR[rs], immediate);
				break;
			case 10:
				sprintf(str,"slti\n\t\tOperation: $%d = $%d < $%d", rt, rs, immediate);//-------------------------------------TESTED
				fputs(str, file);
				GPR[rt] = ula_slt(GPR[rs], immediate);
				break;
			case 11:
				sprintf(str,"sltiu\n\t\tOperation: $%d = $%d < $%d ", rt, rs, immediate); //-------------------------------------TESTED
				fputs(str, file);
				GPR[rt] = ula_sltu(GPR[rs], immediate);
				break;
			case 12:
				sprintf(str,"andi\n\t\tOperation: $%d = $%d and %d", rt, rs, immediate); //-------------------------------------TESTED
				fputs(str, file);
				GPR[rt] = ula_and(GPR[rs], immediate);
				break;
			case 13:
				sprintf(str,"ori\n\t\tOperation: $%d = $%d or $%d", rt, rs, immediate); //------------------------------------- TESTED
				fputs(str, file);
				GPR[rt] = ula_or(GPR[rs], immediate);
				break;
			case 14:
				sprintf(str,"xori\n\t\tOperation: $%d = $%d or $%d", rt, rs, immediate);
				fputs(str, file);
				GPR[rt] = ula_xor(GPR[rs], immediate);
				break;
			case 15:
				sprintf(str,"lui\n\t\tOperation: %d = %d << 16 ", rt, immediate); //------------------------------------- TESTED
				fputs(str, file);
				lui(rt, immediate);
				break;
			case 32:
				sprintf(str,"lb\n\t\tOperation: $%d <- %d($%d) >> 8", rt, immediate, rs);
				fputs(str, file);
				lb(rt, immediate, rs);
				break;
			case 33:
				sprintf(str,"lh\n\t\tOperation: $%d <- %d($%d) >> 16", rt, immediate, rs); 
				fputs(str, file);
				lh(rt, immediate, rs);
				break;
			case 35:
				sprintf(str,"lw\n\t\tOperation: $%d <- %d($%d)", rt, immediate, rs); //-------------------------------------TESTED
				fputs(str, file);
				lw(rt, immediate, rs);
				break;
			case 40:
				sprintf(str,"sb\n\t\tOperation: $%d -> %d($%d)", rt, immediate, rs);
				fputs(str, file);
				sb(GPR[rt], immediate, rs);
				break;
			case 41:
				sprintf(str,"sh\n\t\tOperation: $%d >> 16 -> %d($%d)", rt, immediate, rs);
				fputs(str, file);
				sh(GPR[rt], immediate, rs);
				break;
			case 43:
				sprintf(str,"sw\n\t\tOperation: $%d >> 8-> %d($%d)", rt, immediate, rs);//-------------------------------------TESTED
				fputs(str, file);
				sw(GPR[rt], immediate, rs);
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
				sprintf(str,"j\n\t\tOperation: PC = 0x%04x", address);//-------------------------------------TESTED
				fputs(str, file);
				j(address);
				break;
			case 3:
				sprintf(str,"jal\n\t\tOperation: $ra = 0x%04x and PC = 0x%04x ", PC, address);//-------------------------------------TESTED
				fputs(str, file);
				jal(address);
				break;
		}
	}
	fputs("\n\n", file);
}

//Takes the binary range
int getBinaryRange(int nBits, char signal)
{
	nBits--;
	if(signal == '+')
		return pow(2,nBits) - 1;
	else if(signal == '-'){
		return pow(2,nBits)*(-1);
	}
	return 0;
	
}

//The sum logic in ULA
int ula_add(int op1, int op2)
{
	int sum = op1 + op2;
	char str[50];
	sprintf(str, "= %d + %d = %d", op1, op2, sum);
	fputs(str,file);
	//Check flag conditions
	if(sum == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(sum > getBinaryRange(32, '+')){
		printf("%d %d", sum, getBinaryRange(32, '+'));
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return sum;
}


//The multiplication logic in ULA
int ula_mul(int op1, int op2)
{
	int mult = op1 * op2;
	char str[50];
	sprintf(str, " = %d * %d = %d", op1, op2, mult);
	fputs(str,file);
	//Check flag conditions
	if(mult == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(mult > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return mult;
}
//The ULA division comparation
int ula_div(int op1, int op2)
{
	int div = 0;
	char str[50];
	
	div = op1/op2;
	sprintf(str, "= %d / %d = %d", op1, op2, div);
	fputs(str,file);
	
	if(div == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(div > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	LO = div;
	int mod = (op1%op2);
	HI = mod;
	
	sprintf(str, " | HI = %d LO = %d", HI, LO);
	fputs(str,file);	
	return div;
}

//The ULA equal comparation
int ula_equal(int op1, int op2)
{
	int comparation = 0;
	if(op1 == op2)
		comparation = 1;
		
	if(comparation == 0){
		flags[0] = 1;
	}
	return comparation;
}

//The ULA subtration comparation
int ula_sub(int op1, int op2)
{
	int sub = op1 - op2;
	char str[50];
	sprintf(str, "= %d - %d = %d", op1, op2, sub);
	fputs(str,file);
	//Check flag conditions
	if(sub == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(sub > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return sub;
}

//The beq instruction implementation
void beq(int op1, int op2, int offset)
{
	char str[50];
	sprintf(str, "%d == %d? ", op1, op2);
	fputs(str,file);
	int comparation = ula_equal(op1, op2);
	if(comparation == 1){
		fputs("Yes", file);
		sprintf(str, " -> PC = 0x%04x + (%d)",PC,offset);
		fputs(str,file);
		PC--;
		PC = PC + offset;
	}
	else
		fputs("No", file);
	
}


//The bne instruction implementation
void bne(int op1, int op2, int offset)
{
	char str[50];
	sprintf(str, "%d != %d? ", op1, op2);
	fputs(str,file);
	int comparation = ula_equal(op1, op2);
	if(comparation == 0){
		fputs("Yes (Flag zero activated)", file);
		sprintf(str, " -> PC = 0x%04x + (%d)",PC,offset);
		fputs(str,file);
		PC--;
		PC = PC + offset;
	}
	else
		fputs("No", file);
}

//The slt instruction implementation
int ula_slt(int op1, int op2){
	char str[50];
	sprintf(str, "%d < %d? ", op1, op2);
	fputs(str,file);
	if(op1 < op2){
		fputs(" Yes.", file);
		return 1;
	}
	else
		fputs(" No.", file);
	return 0;
}

//The sltu instruction implementation
unsigned int ula_sltu(unsigned int op1, unsigned int op2){
	char str[50];
	sprintf(str, "%d < %d? ", op1, op2);
	fputs(str,file);
	if(op1 < op2){
		fputs(" Yes.", file);
		return 1;
	}
	else
		fputs(" No.", file);
	return 0;
}

//The j instruction implementation
void j(int address){
	PC = address;
}

void sw(int value, int offset, int baseRegister){
	char str[50];
	int address = GPR[baseRegister];
	sprintf(str, " = %d -> (0x%04x + %d) ", value, address, offset);
	fputs(str,file);
	memory[address + offset] = value;
	
	if(baseRegister == 29){
		int i;
		fputs("\n\t\t\tSTACK\n", file);
		for(i = GPR[30]; i >= GPR[29]; i--){
			sprintf(str, "\t\t\t0x%04x -> %d\n", i, memory[i]);
			fputs(str,file);
		}
	}
	else{
		wordsQuantity++;
		int newAddress = (address + offset);
		if(newAddress > next_free_address)
			next_free_address = newAddress;	
	}
}

void sh(int value, int offset, int baseRegister){
	char str[50];
	int address = GPR[baseRegister];
	sprintf(str, " = %d -> (0x%04x + %d) ", value, address, offset);
	fputs(str,file);
	memory[address + offset] = (value & getBinaryRange(16, '+'));
	
	if(baseRegister == 29){
		int i;
		fputs("\n\t\t\tSTACK\n", file);
		for(i = GPR[30]; i >= GPR[29]; i--){
			sprintf(str, "\t\t\t0x%04x -> %d\n", i, memory[i]);
			fputs(str,file);
		}
	}
	else{
		wordsQuantity++;
		int newAddress = (address + offset);
		if(newAddress > next_free_address)
			next_free_address = newAddress;	
	}
}


void sb(int value, int offset, int baseRegister){
	char str[50];
	int address = GPR[baseRegister];
	sprintf(str, " = %d -> (0x%04x + %d) ", value, address, offset);
	fputs(str,file);
	memory[address + offset] = (value & getBinaryRange(8, '+'));
	
	if(baseRegister == 29){
		int i;
		fputs("\n\t\t\tSTACK\n", file);
		for(i = GPR[30]; i >= GPR[29]; i--){
			sprintf(str, "\t\t\t0x%04x -> %d\n", i, memory[i]);
			fputs(str,file);
		}
	}
	else{
		wordsQuantity++;
		int newAddress = (address + offset);
		if(newAddress > next_free_address)
			next_free_address = newAddress;	
	}
}



void lw(int regist, int offset, int baseRegister){
	char str[50];
	int address = GPR[baseRegister];
	sprintf(str, " = %d <- (0x%04x + %d) ", regist, address, offset);
	fputs(str,file);
	GPR[regist]= memory[address + offset] ;
	if(baseRegister == 29){
		int i;
		fputs("\n\t\t\tSTACK\n", file);
		for(i = GPR[30]; i >= GPR[29]; i--){
			sprintf(str, "\t\t\t0x%04x -> %d\n", i, memory[i]);
			fputs(str,file);
		}
	}
}


void lh(int regist, int offset, int baseRegister){
	char str[50];
	int address = GPR[baseRegister];
	sprintf(str, " = %d <- (0x%04x + %d) ", regist, address, offset);
	fputs(str,file);
	GPR[regist]= (memory[address + offset] & getBinaryRange(16, '+'));
	if(baseRegister == 29){
		int i;
		fputs("\n\t\t\tSTACK\n", file);
		for(i = GPR[30]; i >= GPR[29]; i--){
			sprintf(str, "\t\t\t0x%04x -> %d\n", i, memory[i]);
			fputs(str,file);
		}
	}
}


void lb(int regist, int offset, int baseRegister){
	char str[50];
	int address = GPR[baseRegister];
	sprintf(str, " = %d <- (0x%04x + %d) ", regist, address, offset);
	fputs(str,file);
	GPR[regist]= (memory[address + offset] & getBinaryRange(8, '+'));
	if(baseRegister == 29){
		int i;
		fputs("\n\t\t\tSTACK\n", file);
		for(i = GPR[30]; i >= GPR[29]; i--){
			sprintf(str, "\t\t\t0x%04x -> %d\n", i, memory[i]);
			fputs(str,file);
		}
	}
}

void jal(int address){
	GPR[31] = PC;
	PC = address;
}

void jr(int regist){
	PC = GPR[regist];
}

int ula_clzclo(int op, int op2){
	char str[50];
	int zeros = 0, i, ones = 0;
	int aux = 0;
	for(i = 0; i < 32; i++){
		aux = op & 1;
		op = op >> 1;
		if(aux == 0)
			zeros++;
		else
			ones++;
	}
	if(op2==0){
		sprintf(str, " = %d", zeros);
		fputs(str,file);
		return zeros;
	}
		sprintf(str, " = %d", ones);
		fputs(str,file);
		return ones;
}

unsigned int ula_addu(unsigned int op1, unsigned int op2){
	char str[50];
	unsigned sum = op1 + op2;
	sprintf(str, "= %d + %d = %d", op1, op2, sum);
	fputs(str,file);
	//Check flag conditions
	if(sum == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(sum > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return sum;
}

unsigned subu (unsigned int op1, unsigned int op2){
	char str[50];
	int sub = op1 - op2;
	sprintf(str, "= %d - %d = %d", op1, op2, sub);
	fputs(str,file);
	//Check flag conditions
	if(sub == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(sub > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return sub;
}

int ula_and(int op1, int op2){
	char str[50];
	int ula_and = (op1 & op2);
	sprintf(str, " = %d and %d = %d", op1, op2, ula_and);
	fputs(str,file);
	if(ula_and > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return ula_and;
}

int ula_nor(int op1, int op2){
	int ula_nor = (~(op1 | op2));
	char str[50];
	sprintf(str, " = %d nor %d = %d", op1, op2, ula_nor);
	fputs(str,file);
	if(ula_nor == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(ula_nor > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return ula_nor;
}

int ula_or(int op1, int op2){
	int ula_or = (op1 | op2); 
	char str[50];
	sprintf(str, " = %d or %d = %d", op1, op2, ula_or);
	fputs(str,file);
	if(ula_or == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(ula_or > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return ula_or;
}

int ula_xor(int op1, int op2){
	char str[50];
	int ula_xor = (op1 ^ op2);
	sprintf(str, " = %d xor %d = %d", op1, op2, ula_xor);
	fputs(str,file);
	if(ula_xor == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(ula_xor > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return ula_xor;
}

unsigned int ula_divu(unsigned int op1, unsigned int op2){
	int div = 0;
	char str[50];
	div = op1/op2;
	sprintf(str, " = %d / %d = %d", op1, op2, div);
	fputs(str,file);
	if(div == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(div > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	LO = div;
	int mod = (op1%op2);
	HI = mod;
	
	sprintf(str, " | HI = %d LO = %d", HI, LO);
	fputs(str,file);
	return div;
}

unsigned int ula_multu(unsigned int op1, unsigned int op2){
	int mult = op1 * op2;
	char str[50];
	sprintf(str, " = %d * %d = %d", op1, op2, mult);
	fputs(str,file);
	//Check flag conditions
	if(mult == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(mult > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return mult;
}

int ula_sllv(int op1, int op2){
	char str[50];
	int sllv = (op1 << op2);
	sprintf(str, " = %d << %d = %d", op1, op2, sllv);
	fputs(str,file);
	return sllv;
}

int ula_srlv(int op1, int op2){
	char str[50];
	int srlv = (op1 >> op2);
	sprintf(str, " = %d >> %d = %d", op1, op2, srlv);
	fputs(str,file);
	return srlv;
}

int movefromHILO(int op){
	char str[50];
	if(op==1){
		sprintf(str, " = HI = %d", HI);
		fputs(str,file);
		return HI;
	}else{
		sprintf(str, " = LO = %d", LO);
		fputs(str,file);
		return LO;
	}
}

void movetoHILO(int value, int op){
	char str[50];
	if(op==1){
		sprintf(str, " = HI = rs = %d", value);
		fputs(str,file);
		HI = value;
	}else{
		sprintf(str, " = LO = rs = %d", value);
		fputs(str,file);
		LO = value;
	}
}

void ula_mult(int op1, int op2){
	long long int produt = op1 * op2;
	char str[50];

	if(produt < getBinaryRange(32, '+')){
		sprintf(str, " %d * %d = %d -> HI = 0, LO = %d",op1, op2, produt, produt);
		fputs(str,file);
		LO = produt;
		HI = 0;
	}else{	
	LO = produt & 4294967295;
	HI = produt >> 32;	
	sprintf(str, " %d * %d = %d -> HI = 0, LO = %d",op1, op2, produt, produt);
	fputs(str,file);
	}
}

void jalr(int rd, int rs){
	GPR[rd] = PC;
	PC = GPR[rs];
}


void lui(int op1, int op2){
	char str[50];
	GPR[op1] = op2;
	GPR[op1] = GPR[op1] << 16;
	sprintf(str, "%d = %d << 16 ", op1, op2);
	fputs(str,file);
}

void ula_seh(int op1, int op2){
	char str[50];
	GPR[op1] = op2;
	GPR[op1] = (GPR[op1] & getBinaryRange(16, '+'));
	sprintf(str, "%d = %d << 16 ", op1, op2);
	fputs(str,file);
}

void ula_seb(int op1, int op2){
	char str[50];
	GPR[op1] = op2;
	GPR[op1] = (GPR[op1] & getBinaryRange(8, '+'));
	sprintf(str, "%d = %d << 24 ", op1, op2);
	fputs(str,file);
	
}

unsigned int ula_subu( unsigned int op1, unsigned int op2){
	unsigned int sub = op1 - op2;
	char str[50];
	sprintf(str, "= %d - %d = %d", op1, op2, sub);
	fputs(str,file);
	//Check flag conditions
	if(sub == 0){
		flags[0] = 1;
		fputs(" (Flag zero activated)", file);
	}
	else if(sub > getBinaryRange(32, '+')){
		flags[3] = 1;
		fputs(" (Flag overflow activated)", file);
	}
	return sub;
}

void madd(int op1, int op2){
	long long int produt = op1 * op2;
	char str[50];

	if(produt < getBinaryRange(32, '+')){
		sprintf(str, " %d * %d = %d -> HI = 0, LO = %d",op1, op2, produt, produt);
		fputs(str,file);
		LO += produt;
		HI = 0;
	}else{	
	LO += (produt & 4294967295);
	HI += (produt >> 32);	
	sprintf(str, " %d * %d = %d -> HI = 0, LO = %d",op1, op2, produt, produt);
	fputs(str,file);
	}
}

void maddu(unsigned int op1, unsigned int op2){
	
	long long unsigned int produt = op1 * op2;
	char str[50];
	if(produt < getBinaryRange(32, '+')){
		sprintf(str, " %d * %d = %d -> HI = 0, LO = %d",op1, op2, produt, produt);
		fputs(str,file);
		LO += produt;
		HI = 0;
	}else{	
	LO += (produt & 4294967295);
	HI += (produt >> 32);	
	sprintf(str, " %d * %d = %d -> HI = 0, LO = %d",op1, op2, produt, produt);
	fputs(str,file);
	}
}


void msubu(unsigned int op1, unsigned int op2){
	
	long long unsigned int produt = op1 * op2;
	char str[50];

	if(produt < getBinaryRange(32, '+')){
		sprintf(str, " %d * %d = %d -> HI = 0, LO = %d",op1, op2, produt, produt);
		fputs(str,file);
		LO -= produt;
		HI = 0;
	}else{	
	LO -= (produt & 4294967295);
	HI -= (produt >> 32);	
	sprintf(str, " %d * %d = %d -> HI = 0, LO = %d",op1, op2, produt, produt);
	fputs(str,file);
	}
}

void msub(int op1, int op2){
	
	long long int produt = op1 * op2;
	char str[50];

	if(produt < getBinaryRange(32, '+')){
		sprintf(str, " %d * %d = %d -> HI = 0, LO = %d",op1, op2, produt, produt);
		fputs(str,file);
		LO -= produt;
		HI = 0;
	}else{	
	LO -= (produt & 4294967295);
	HI -= (produt >> 32);	
	sprintf(str, " %d * %d = %d -> HI = 0, LO = %d",op1, op2, produt, produt);
	fputs(str,file);
	}
}

int rotr(int op2, int offset){
	int temp1;
	int temp2 = (op2 & getBinaryRange(offset, '+'));
	
	temp2 = temp2 << (32 - offset); 
	
	op2 = op2  >> offset;
	
	temp1 = temp2 | op2;
	
	return temp1;
	
}

int rotrv(int op2, int op3){
	int temp1;
	int temp2 = (op2 & getBinaryRange(op3, '+'));
	
	
	temp2 = temp2 << (32 - op3); 
	op2 = op2  >> op3;
	
	temp1 = temp2 | op2;
	
	return temp1;
}


void bgtz(int op1, int offset){
	
	if(op1 > 0){
		PC = PC + offset;
	}
}

void bltz(int op1, int offset){
	
	if(op1 < 0){
		PC = PC + offset;
	}
}


