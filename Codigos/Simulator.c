#include <stdio.h>
#include <stdlib.h>

int callLoader();
void showMemory();
void cleanMemory();

//The processor memory
int memory[16384];
//General purpose registers
int GPR[32];
//The next memory address to program
unsigned int program_memory = 0;
//Base register to data memory
unsigned int dBaseRegister = 0;
//PC register
unsigned int PC = 0;

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
        }
        //Error
        else
        {

        }
        cleanMemory();
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
    printf("\nUSED MEMORY:\t%.4fKB\n-----------------------------\n\tAddress\t\tValue\n", memoryUsage/1024);
    for(i; i <= program_memory; i++){
    	if(PC == i)
    		printf("PC ->");
    	if(dBaseRegister == i)
    		printf("DBR ->");
    	if(GPR[30] == i)
    		printf("$fp ->");
    	if(GPR[29] == i && GPR[30] != GPR[29])
    		printf("$sp ->");
    	printf("\t0x%04x\t\t%d\n", i, memory[i]);
	}
	printf("-----------------------------\n");
}

//Cleans the memory
void cleanMemory()
{
    int i = 0;
    for(i; i < program_memory; i++)
        memory[i] = 0;
    program_memory = 0;
}
