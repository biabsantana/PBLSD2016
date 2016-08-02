#include <stdio.h>
#include <stdlib.h>

int callLoader();
void showMemory();
void cleanMemory();
void cleanRegisters();
int executeProgram();
int callControlUnit();

//------------------ULA OPERATIONS-----------------------------------
int add(int op1, int op2);
unsigned int addu(unsigned int op1, unsigned int op2);
int clz(int op);
int clo(int op);
int sub (int op1, int op2);
unsigned subu (unsigned int op1, unsigned int op2);
int seh(int op);
int seb(int op);
int _and(int op1, int op2);
int nor(int op1, int op2);
int _or(int op1, int op2);
int _xor(int op1, int op2);
int div(int op1, int op2);
unsigned int divu(unsigned int op1, unsigned int op2);
int mult(int op1, int op2);
unsigned int multu(unsigned int op1, unsigned int op2);
int sllv(int op1, int op2);
int srlv(int op1, int op2);
int srav(int op1, int op2);
int rotrv(int op1, int op2);
int slt(int op1, int op2);
unsigned int sltu(unsigned int op1, unsigned int op2);
int equal(int op1, int op2);
//------------------ULA OPERATIONS-----------------------------------


//------------------BRANCHS OPERATIONS-------------------------------

//------------------BRANCHS OPERATIONS-------------------------------


//------------------LOAD/STORE OPERATIONS----------------------------

//------------------LOAD/STORE OPERATIONS----------------------------


//------------------JUMP OPERATIONS-------------------------------

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
int IR = 0;
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
            success = executeProgram();
            
            //OK
			if(success == 1){
            	
			}
			//Error
			else{
				
			}
        }
        //Error
        else
        {

        }
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
    printf("\nUSED MEMORY:\t%.4fKB\n----------------------------------\n\tAddress\t\tValue\n", memoryUsage/1024);
    for(i; i <= next_free_address; i++){
    	if(PC == i)
    		printf("PC ->");
    	if(GPR[28] == i && GPR[28] != PC)
    		printf("$gp ->");
    	if(GPR[30] == i && GPR[28] != GPR[30])
    		printf("$fp ->");
    	if(GPR[29] == i && GPR[30] != GPR[29])
    		printf("$sp ->");
    	printf("\t0x%04x\t\t%d\n", i, memory[i]);
	}
	printf("----------------------------------\n");
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
int executeProgram()
{
	int success = 1;
	//Executes the program while PC address is a instruction address on memory
	while(PC != GPR[28]){
		//Takes the memory value and puts on IR
		IR = memory[PC];
		//Calls the control unit
		success = callControlUnit();
		if(success == 0)
			return 0;
	}
	return 1;
}

//Calls the control unit to start the instruction execution
int callControlUnit()
{
	//Shift the IR to find the opcode[31...26]
	unsigned int opcode = IR >> 26;
	
	//R instruction
	if(opcode == 0){
		//Find the R instruction function [5...0]
		unsigned int function = IR & 63;
		//Find the R instruction shamt [10...6]
		unsigned int shamt = (IR >> 6) & 31;
		//Find the R instruction destiny register [15...11]
		unsigned int reg_dest = (IR >> 11) & 31;
		//Find the R instruction operand register 2 [20...16]
		unsigned int reg_op2 = (IR >> 16) & 31;
		//Find the R instruction operand register 1[25...21]
		unsigned int reg_op1 = (IR >> 21) & 31;
		
		//Calls the specific instruction with your parameters
		switch(function){
			//COLOCAR EM CADA CASE DESSE UMA CHAMADA DE UMA FUN플O QUE IR� REPRESENTAR A INSTRU플O DO TIPO R
			case 1:
				
				break;
		}
		printf("R - %d %d %d %d %d %d\n", opcode, reg_op1, reg_op2, reg_dest, shamt, function);
	}
	//I instruction
	else if((opcode >= 1 && opcode <= 15) || (opcode >= 18 && opcode <= 23)){
		//Find the I instruction immediate [15...0]
		short int constant = IR & 65535;
		//Find the I instruction destiny register [20...16]
		unsigned int reg_dest = (IR >> 16) & 31;
		//Find the I instruction operand register [25...21]
		unsigned int reg_op = (IR >> 21) & 31;
		
		//Calls the specific instruction with your parameters
		switch(opcode){
			//COLOCAR EM CADA CASE DESSE UMA CHAMADA DE UMA FUN플O QUE IR� REPRESENTAR A INSTRU플O DO TIPO I
			case 1:
				
				break;
		}
		printf("I - %d %d %d %d\n", opcode, reg_op, reg_dest, constant);
	}
	//J instruction
	else if(opcode == 16 || opcode == 17){
		//Find the J instruction address [25...0]
		unsigned int address = IR & 67108863;

		//Calls the specific instruction with your parameters
		switch(opcode){
			//COLOCAR EM CADA CASE DESSE UMA CHAMADA DE UMA FUN플O QUE IR� REPRESENTAR A INSTRU플O DO TIPO J
			case 16:
				
				break;
			case 17:
				
				break;
		}
		printf("J - %d %d\n", opcode, address);
	}
	//TESTE
	PC++;
	return 1;
}

int add(int op1, int op2)
{
	return (op1 + op2); 
}

unsigned int addu(unsigned int op1, unsigned int op2)
{
	return (op1 + op2); 
}

int clz(int op)
{
	int zeros = 0;
	while(op>0){
		if(op%10 == 0){
			zeros++;
			op = op/10;
		}
	}
	return zeros;
}

int clo(int op)
{
	int ums = 0;
	while(op>0){
		if(op%10 == 1){
			ums++;
			op = op/10;
		}
	}
	return ums;
}

int sub(int op1, int op2)
{
	return (op1 - op2); 
}

unsigned int subu(unsigned int op1, unsigned int op2)
{
	return (op1 - op2); 
}
