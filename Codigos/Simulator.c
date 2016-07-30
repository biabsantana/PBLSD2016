#include <stdio.h>
#include <stdlib.h>

int callLoader();
void showMemory();
void cleanMemory();
void cleanRegisters();
int executeProgram();
int callControlUnit();

//The processor memory
int memory[16384];
//General purpose registers
int GPR[32];
//The next memory address to program
unsigned int program_memory = 0;
//Base register to data memory
unsigned int dBaseRegister = 0;
//Program Counter register
unsigned int PC = 0;
//Instruction register
int IR = 0;

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
        	dBaseRegister = instruction;
        	numberOfInstructions++;
		}
        	
		else{
			//Put the instruction on memory and increments the next memory address
        	memory[program_memory] = instruction;
        	program_memory++;	
		}
        printf("\n");
    }
    
    //Defines the $sp and $fp address
    GPR[29] = program_memory;
    GPR[30] = GPR[29];    
	fclose(file);
    return 1;
}

//Shows all values in memory and your addresses
void showMemory()
{
    int i = 0;
    float memoryUsage = (program_memory*4);
    printf("\nUSED MEMORY:\t%.4fKB\n----------------------------------\n\tAddress\t\tValue\n", memoryUsage/1024);
    for(i; i <= program_memory; i++){
    	if(PC == i)
    		printf("PC ->");
    	if(dBaseRegister == i && dBaseRegister != PC)
    		printf("DBR ->");
    	if(GPR[30] == i && dBaseRegister != GPR[30])
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
    for(i; i < program_memory; i++)
        memory[i] = 0;
    program_memory = 0;
}

//Cleans all processor registers
void cleanRegisters()
{
	int i = 0;
	for(i; i < 32; i++)
		GPR[i] = 0;
	PC = 0;
	IR = 0;
	dBaseRegister = 0;
}

//Execute the program loaded on memory
int executeProgram()
{
	int success = 1;
	//Executes the program while PC address is a instruction address on memory
	while(PC != dBaseRegister){
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
		//Find the R instruction operand register 2 [15...11]
		unsigned int reg_op2 = (IR >> 11) & 31;
		//Find the R instruction operand register 1 [20...16]
		unsigned int reg_op1 = (IR >> 16) & 31;
		//Find the R instruction destiny register [25...21]
		unsigned int reg_dest = (IR >> 21) & 31;
		
		//Calls the specific instruction with your parameters
		switch(function){
			//COLOCAR EM CADA CASE DESSE UMA CHAMADA DE UMA FUN플O QUE IR� REPRESENTAR A INSTRU플O DO TIPO R
			case 1:
				
				break;
		}
		printf("R - %d %d %d %d %d %d\n", opcode, reg_dest, reg_op1, reg_op2, shamt, function);
	}
	//I instruction
	else if((opcode >= 1 && opcode <= 15) || (opcode >= 18 && opcode <= 23)){
		//Find the I instruction immediate [15...0]
		short int constant = IR & 65535;
		//Find the I instruction operand register [20...16]
		unsigned int reg_op = (IR >> 16) & 31;
		//Find the I instruction destiny register [20...16]
		unsigned int reg_dest = (IR >> 21) & 31;
		
		//Calls the specific instruction with your parameters
		switch(opcode){
			//COLOCAR EM CADA CASE DESSE UMA CHAMADA DE UMA FUN플O QUE IR� REPRESENTAR A INSTRU플O DO TIPO I
			case 1:
				
				break;
		}
		printf("I - %d %d %d %d\n", opcode, reg_dest, reg_op, constant);
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
	return 1;
}
