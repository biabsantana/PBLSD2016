#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "uthash.h"
#include <math.h>

//Functions declarations-------------------------------------------------------------------------------------------------
int assemblyChoice();
void createRegisters();
void addRegister(char key[], int value);
char* findRegister(char key[]);
void addLabel(char key[], int value);
char* findLabel(char key[]);
void addInstruction(char mnemonic[], char data[]);
char* findInstruction(char mnemonic[]);
void createInstructions();
void addCommand(char line[], int lineNumber);
void cleanCommands();
char* strrev(char *str);
char* removeLast(char *str);
char* leftClean(char *str);
char* ignoreComments (char *line);
char* verifyLine(char *aux);
int getDirective(char *directive);
int checksAssembly();
void informError(int code, int lineNumber);
int verifyAndTranslate();
char* checkData(char *line);
char* checkInstruction(char line[], int currentAddress);
char* getMnemonic(char instruction[]);
char* completeBinary(int numberOfBits, char binary[]);
char* mountBinary(char instruction[],int nRegisters,int nConstants,int nLabels, char type, char *opcode, char *function , int currentAddress);
void concatBinary(int binaryIndex, int nBits, char binary[], char param[]);
void writeOnFile();
int getBinaryRange(int nBits, char signal);
//-----------------------------------------------------------------------------------------------------------------------

//Hash created to save the assembly code labels and the processor registers.
struct hash
{
    char key[100];
    int value;
    UT_hash_handle hh;
};

//Hash created to save the processor instructions.
struct instruction
{
    char mnemonic[10];
    char data[16];
    UT_hash_handle hh;
};

//This list save all assembly instructions in the file.
struct command
{
    char line[100];
    int lineNumber;
    struct command *next;
};

//The first command on file
struct command *first = NULL;
//Save the processor registers
struct hash *registers = NULL;
//Save the assembly code labels
struct hash *labels = NULL;
//The processor instructions
struct instruction *instructions = NULL;

//Save the input assembly file name
char urlInputFile[100];
//Number of instructions presents on assembly code
int numberOfInstructions = 0;
//Number of data presents on assembly code
int numberOfData = 0;

void main()
{
    //Informs error on code (0 - correct, 1 - error)
    int fileError = 0;
    //User choice
    char choice;

    //Creates the registers and instructions
    createRegisters();
    createInstructions();

    while(1){
        //Wait for a correct assembly file
        while(assemblyChoice() == 0)
            system("cls");

        printf("\n*Press Y to assemble the file or other button to entry with other file*\n");
        choice = getch();

        if(choice == 'y'){
            //Checks the assembly program
            fileError = checksAssembly();

            //If the program not has error
            if(fileError == 0){

                fileError = verifyAndTranslate();

                if(fileError == 1){
                    //Calculates the program memory usage in Bytes
                    float memoryUsageB = ((numberOfData + numberOfInstructions)*4);
                    printf("\nPROGRAM ASSEMBLED\n");
                    printf("Number of instructions: %d\nNumber of data: %d\nMemory usage: %.4fKB\n", numberOfInstructions, numberOfData, memoryUsageB/1024);
                    writeOnFile();
                }

            }

            printf("\n*Press any button to entry with other file*\n");
            getch();

            //Clean all past data
            cleanCommands();
            labels = NULL;
            numberOfData = 0;
            numberOfInstructions = 0;
            fileError = 0;
        }
        system("cls");
    }
}

//Chooses and shows the assembly file.
int assemblyChoice()
{
    //Get the file line
    char line[300];
    //Informs the file line number
    int lineNumber = 1;
    FILE *file = NULL;

    printf("TEC499 - ASSEMBLER\n\n");

    //Gets the file name
    printf("Into the .asm name: ");
    gets(urlInputFile);
    file = fopen(urlInputFile, "r");

    //Error on file open
    if(file == NULL){
        informError(0, 0);
        free(file);
        return 0;
    }

    system("cls");
    printf("*SOURCE CODE*\n\n");

    //Reads all file
    while(fgets(line, sizeof(line), file)!= NULL)
    {
        //Shows the file line and your number
        printf("%d\t%s", lineNumber, line);
        lineNumber++;
    }
    fclose(file);
    return 1;
}

//Add a register in the registers hash.
void addRegister(char key[], int value)
{
    struct hash *reg;
    //Verifies if the register already exists on hash
    HASH_FIND_STR(registers, key, reg);

    //If not exists, creates.
    if(reg == NULL)
    {
        reg = (struct hash*)malloc(sizeof(struct hash));
        strncpy(reg->key, key, 100);
        reg->value = value;
        HASH_ADD_STR(registers, key, reg);
    }
}

//Get the register code
char* findRegister(char key[])
{           
	while(key[0] == ' ' || key[0] == '\t')
        key = ++key;
	
	int lastIndex = strlen(key) - 1;
	while(key[lastIndex] == ' ' || key[lastIndex] == '\t'){
        key[lastIndex] = '\0';
        lastIndex--;
	}
        
    struct hash *r;
    static char buffer[5];
    HASH_FIND_STR(registers, key, r);
	
    if(r != NULL){
        itoa(r->value, buffer, 2);
        if(strlen(buffer) < 5)
            strcpy(buffer, completeBinary(5, buffer));
        buffer[5] = '\0';
        return buffer;
    }

    return "NULL";
}

//Add a label in the labels hash
void addLabel(char key[], int value)
{
    while(key[0] == ' ' || key[0] == '\t')
        key = ++key;
        
    struct hash *labl;
    //Verifies if the register already exists on hash
    HASH_FIND_STR(labels, key, labl);

    //If not exists, creates.
    if(labl == NULL)
    {       
        labl = (struct hash*)malloc(sizeof(struct hash));
        strncpy(labl->key, key, 100);
        labl->value = value;
        HASH_ADD_STR(labels, key, labl);
    }
}

//Get the label code
char* findLabel(char key[])
{
	while(key[0] == ' ' || key[0] == '\t')
        key = ++key;
        
    struct hash *lbl;
    static char buffer[16];
    HASH_FIND_STR(labels, key, lbl);

    if(lbl != NULL){
        itoa(lbl->value, buffer, 2);
        if(strlen(buffer) < 16)
            strcpy(buffer, completeBinary(16, buffer));
        buffer[16] = '\0';
        return buffer;
    }
    return "NULL";
}

//Add a instruction on assembler
void addInstruction(char mnemonic[], char data[])
{
    struct instruction *inst;
    HASH_FIND_STR(instructions, mnemonic, inst);

    if(inst == NULL)
    {
        inst = (struct instruction*)malloc(sizeof(struct instruction));
        strncpy(inst->mnemonic, mnemonic, 10);
        strncpy(inst->data, data, 16);
        HASH_ADD_STR(instructions, mnemonic, inst);
    }
}

//Find a processor instruction
char* findInstruction(char mnemonic[])
{
    struct instruction *i;
    HASH_FIND_STR(instructions, mnemonic, i);
    if(i != NULL)
        return i->data;
    return "NULL";
}

//Creates all processor instructions
void createInstructions()
{
    /// Data format: [0] - Type(R, I or J), [1] - Number of registers, [2] - number of constants, [3] - labels
    
	//R - instructions------------------------------------
	//Arithmetic
    addInstruction("add",  "R300000000100000");
    addInstruction("addu", "R300000000100001");
    addInstruction("clz",  "R200011100100000");
    addInstruction("clo",  "R200011100100001");
    addInstruction("sub",  "R300000000100010");
    addInstruction("subu", "R300000000100011");
	addInstruction("seb",  "R200011111100000");
	addInstruction("seh",  "R200011111100000");
    //Logic
    addInstruction("and",  "R300000000100100");
    addInstruction("nor",  "R300000000100111");
    addInstruction("wsbh", "R200011111100000");
    addInstruction("or",   "R300000000100101");
    addInstruction("xor",  "R300000000100110");
    addInstruction("ext",  "R220011111000000");
    addInstruction("ins",  "R220011111000100");
    //Multiplication and Division
    addInstruction("div",  "R200000000011010"); 
    addInstruction("divu", "R200000000011011"); 
    addInstruction("madd", "R200011100000000");  
    addInstruction("maddu","R200011100000001"); 
    addInstruction("msub", "R200011100000100");
    addInstruction("msubu","R200011100000101"); 
    addInstruction("mul",  "R300011100000010");
    addInstruction("mult", "R200000000011000"); 
    addInstruction("multu","R200000000011001"); 
    //Shift and Rotate
    addInstruction("sll",  "R210000000000000");
    addInstruction("sllv", "R300000000000100");
    addInstruction("srl",  "R210000000000010");
    addInstruction("sra",  "R210000000000011");
    addInstruction("srav", "R300000000000111"); 
    addInstruction("srlv", "R300000000000110");
    addInstruction("rotr", "R210000000000010");
    addInstruction("rotrv","R300000000000110");
    //Conditionals and move
    addInstruction("slt",  "R300000000101010");
    addInstruction("sltu", "R300000000101011");
    addInstruction("movn", "R300000000001011");
    addInstruction("movz", "R300000000001010");
    //Acc access
    addInstruction("mfhi", "R100000000010000");
    addInstruction("mflo", "R100000000010010");
    addInstruction("mthi", "R100000000010001");
    addInstruction("mtlo", "R100000000010011");
    //Branches and jump
    addInstruction("jr",   "R100000000001000");
    addInstruction("jalr", "R200000000001001"); 
    
    //I - instructions------------------------------------
    //Arithmetic
    addInstruction("addi",  "I210001000");
    addInstruction("addiu", "I210001001");
    addInstruction("lui",   "I110001111");
    //Logic
    addInstruction("andi",  "I210001100");
    addInstruction("ori",   "I210001101");
    addInstruction("xori",  "I210001110");	
	//Conditionals and move
	addInstruction("slti",  "I210001010");
	addInstruction("sltiu", "I210001011");
	//Branches and jump
	addInstruction("beq",   "I201000100");
	addInstruction("bgtz",  "I101000111");
	addInstruction("bne",   "I201000101");
	addInstruction("bltz",  "I101000001");
	//Load and store
	addInstruction("lb",    "I210100000");
	addInstruction("lw",    "I210100011");
	addInstruction("lh",    "I210100001");
	addInstruction("sb",    "I210101000");
	addInstruction("sh",    "I210101001");
	addInstruction("sw",    "I210101011");
	
	//J - instructions------------------------------------
	//Branches and jump
	addInstruction("j",     "J001000010");
	addInstruction("jal",   "J001000011");

}

//Add in the list a read command
void addCommand(char line[], int lineNumber)
{
    //New command
    struct command *newCommand;
    //List aux
    struct command *aux;

    newCommand = (struct command *) malloc(sizeof(struct command));
    strncpy(newCommand->line, line, 100);
    newCommand->next = NULL;
    newCommand->lineNumber = lineNumber;

    //If the list is empty
    if(first == NULL)
        first = newCommand;

    //Else
    else{
        aux = first;

        while(aux->next != NULL)
            aux = aux->next;

        aux->next = newCommand;
    }
}

//Prints all read commands and clean the list
void cleanCommands()
{
    struct command *com, *aux;
    com = (struct command*)malloc(sizeof(struct command));

    com = first;
    while(com != NULL){
        com = com->next;
        free(com);
    }
    first = NULL;
}

//Creates all 32 representations of processor registers
void createRegisters()
{
    addRegister("$zero", 0);
    addRegister("$at", 1);
    addRegister("$v0", 2);
    addRegister("$v1", 3);
    addRegister("$a0", 4);
    addRegister("$a1", 5);
    addRegister("$a2", 6);
    addRegister("$a3", 7);
    addRegister("$t0", 8);
    addRegister("$t1", 9);
    addRegister("$t2", 10);
    addRegister("$t3", 11);
    addRegister("$t4", 12);
    addRegister("$t5", 13);
    addRegister("$t6", 14);
    addRegister("$t7", 15);
    addRegister("$s0", 16);
    addRegister("$s1", 17);
    addRegister("$s2", 18);
    addRegister("$s3", 19);
    addRegister("$s4", 20);
    addRegister("$s5", 21);
    addRegister("$s6", 22);
    addRegister("$s7", 23);
    addRegister("$t8", 24);
    addRegister("$t9", 25);
    addRegister("$k0", 26);
    addRegister("$k1", 27);
    addRegister("$gp", 28);
    addRegister("$sp", 29);
    addRegister("$fp", 30);
    addRegister("$ra", 31);
}

//Inverts a string.
char *strrev(char *str)
{
    char *p1, *p2;

    if (! str || ! *str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}

//Removes the string '\n'.
char* removeLast(char *str)
{
    str = strrev(str);
    if (str[0] == '\n')
        str++;
    return strrev(str);
}

//Clean all unnecessary spaces on string
char* leftClean(char *str)
{
    int i = 0;
    while(str[i] != '\0' && (str[i] == ' ' || str[i] == '\t'))
        i++;

    if (str[i] == '\0' || str[i] == ';')
        return NULL;

    return strchr(str, str[i]);
}

//Ignore all code comments.
char* ignoreComments (char *line)
{
    //If the line starts on comment
    if (line[0] == ';')
        return NULL;

    line = strrev(line);
    char *aux = strchr(line, ';');
    while (aux != NULL)
    {
        line = ++aux;
        aux = strchr(line, ';');
    }
    line = leftClean(line);
    if (line != NULL)
        line = strrev(line);

    return line;
}

//Verifies all code lines(taking comments, labels and blank lines).
char* verifyLine(char *aux)
{
    //Takes the '\n'
    char *line = removeLast(aux);

    //If is not a blank line
    if (line != NULL)
        line = ignoreComments(line);

    //If is not only a comment
    if(line == NULL)
        return NULL;

    //If there is a label
    char *instruction = strchr(line, ':');
    if (instruction != NULL)
    {
        //Take the label
        char* label = strchr(strrev(line), ':');
        label = strrev(strchr(label, label[1]));

        //Inserts on hash
        addLabel(label, numberOfData + numberOfInstructions);
        //If is not a instruction too
        if (instruction[1] == '\0')
            return NULL;

        line = strchr(strrev(line), ':');
        //right clean
        int i = 0;
        line = ++line;
        while(line[i] == ' ')
            line = ++line;

    }
    line = leftClean(line);
    if(line == NULL)
        return NULL;

    return line;
}

//Informs the directive code of the read line.
int getDirective(char *directive)
{
    char aux[100];
    //If a directive, take the directive
    if(directive[0] == '.'){
        int i = 0;
        while(directive[i] != ' ' && directive[i] != '\0' && directive[i] != '\t'){
            aux[i] = directive[i];
            i++;
        }
        aux[i] = '\0';
    }
    //If is a instruction or label
    else
        return -1;

    //Choose a directive
    if(strcmp(aux, ".module") == 0)
        return 0;
    else if(strcmp(aux, ".pseg") == 0)
        return 1;
    else if(strcmp(aux, ".dseg") == 0)
        return 2;
    else if(strcmp(aux, ".word") == 0)
        return 3;
    else if(strcmp(aux, ".end") == 0)
        return 4;
    else
        return -2;
}

//Does the assembly code verification, counting the instructions and data number.
int checksAssembly()
{
    FILE *file = NULL;
    char *line;
    char aux[300];

    //Number of lines
    int lineNumber = 1;
    //Counts the program lines(only instructions, directives and labels)
    int lineCount = 0;
    //Tells which part of the reader program is
    int module = 0, end = 0, pseg = 0, dseg = 0;
    //Save the directive code
    int directiveCode = -2;

    int instructionState = 0;

    file = fopen(urlInputFile, "r");

    while(fgets(aux, sizeof(aux), file)!= NULL)
    {
        //Verifies the line
        line = verifyLine(aux);

        //If is anything
        if(line!=NULL)
        {
            //Reads the directive code
            directiveCode = getDirective(line);

            //Asserts that the first instructions are the .module
            if(lineCount == 0 && directiveCode != 0){
                informError(1, lineNumber);
                fclose(file);
                return 1;
            }
            //If is .module
            if(directiveCode == 0){
                module = 1;
                end = 0;
            }
            //If is .pseg
            else if(directiveCode == 1){
                pseg = 1;
                dseg = 0;
            }
            //If is .dseg
            else if(directiveCode == 2){
            	if(pseg == 0){
            		informError(6, lineNumber);
            		fclose(file);
                	return 1;
				}
                pseg = 0;
                dseg = 1;
            }
            //If is .end
            else if(directiveCode == 4){
                module = 0;
                end = 1;
                lineCount = 0;
            }

            //If is a wrong directive
            else if(directiveCode == -2){
                informError(3, lineNumber);
                fclose(file);
                return 1;
            }

            //If is a instruction increments the number of instruction
            if(pseg == 1 && module == 1 && line[0] != '.' && directiveCode != 3){
                addCommand(line, lineNumber);
                numberOfInstructions++;
            }

            else if(pseg == 1 && module == 1 && line[0] == '.' && directiveCode == 3){
                informError(4, lineNumber);
                fclose(file);
                return 1;
            }

            //If is a word increments the number of instruction
            if(dseg == 1 && module == 1 && directiveCode == 3){
                addCommand(line, lineNumber);
                numberOfData++;
            }

            else if(dseg == 1 && module == 1 && directiveCode == -1){
                informError(5, lineNumber);
                fclose(file);
                return 1;
            }

            //Checks if the memory(64KB -> 16384 32-bits address) was exceeded
            if((numberOfData + numberOfInstructions) > 16384){
                informError(2, lineNumber);
                fclose(file);
                return 1;
            }

            if(end != 1)
                lineCount++;
        }
        lineNumber++;
    }
    fclose(file);
    return 0;
}

//Informs the program read error.
void informError(int code, int lineNumber)
{
    printf("\n-------------------------------------------------------------------------------------------------------------------");
    printf("\nPROGRAM NOT ASSEMBLED");

    if(code == 0)
        printf("\n-> ERROR(0): THIS FILE DOESN'T EXISTS");
    else if(code == 1)
        printf("\n-> ERROR(1): THE FILE CONTAINS INSTRUCTIONS OUT OF THE .module DECLARATION - Line %d",lineNumber);
    else if(code == 2)
        printf("\n-> ERROR(2): THIS PROGRAM EXCEEDS THE MEMORY(64KB) - Line %d",lineNumber);
    else if(code == 3)
        printf("\n-> ERROR(3): WRONG DIRECTIVE. ONLY ACCEPT(.module, .dseg, .pseg, .end, .word) - Line %d",lineNumber);
    else if(code == 4)
        printf("\n-> ERROR(4): WORD DECLARATION ON .pseg - Line %d",lineNumber);
    else if(code == 5)
        printf("\n-> ERROR(5): INSTRUCTION ON .dseg - Line %d",lineNumber);
    else if(code == 6)
        printf("\n-> ERROR(15): .dseg CAN'T STAY BEFORE .pseg - Line %d",lineNumber);

}

//Verifies all instructions and data on list and translate to binary
int verifyAndTranslate()
{
    struct command *com, *aux;
    char result[32];
    int currentAddress = 0;

    com = (struct command*)malloc(sizeof(struct command));
    com = first;
    while(com != NULL){
        //Directive
        if(getDirective(com->line) == 3){
            //printf("\n\nDado: (%s)",com->line);
            strcpy(com->line, checkData(com->line));
            if(strcmp(com->line, "0") == 0){
                printf("\n-> ERROR(6): INVALID VALUE - Line %d", com->lineNumber);
                return 0;
            }
            if(strcmp(com->line, "1") == 0){
                printf("\n-> ERROR(7): VALUE OVERFLOW - Line %d", com->lineNumber);
                return 0;
            }
            //printf("\n%s (%d bits)\n",com->line, strlen(com->line));
        }
        //Instruction
        else{
            //printf("\n\nInstrucao: (%s)",com->line);
            strcpy(com->line, checkInstruction(com->line, currentAddress));
            if(strcmp(com->line, "1") == 0){
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(8): INVALID REGISTER - Line %d", com->lineNumber);
                return 0;
            }
            else if(strcmp(com->line, "2") == 0){
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(9): INVALID LABEL - Line %d", com->lineNumber);
                return 0;
            }
            else if(strcmp(com->line, "3") == 0){
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(10): INVALID CONSTANT - Line %d", com->lineNumber);
                return 0;
            }
            else if(strcmp(com->line, "4") == 0){
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(11): INVALID MNEMONIC - Line %d", com->lineNumber);
                return 0;
            }

            else if(strcmp(com->line, "5") == 0){
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(12): MISSING PARAMETERS - Line %d", com->lineNumber);
                return 0;
            }
            else if(strcmp(com->line, "6") == 0){
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(13): CONSTANT OVERFLOW - Line %d", com->lineNumber);
                return 0;
            }
            else if(strcmp(com->line, "7") == 0){
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(14): ONLY UNSIGNED CONSTANT - Line %d", com->lineNumber);
                return 0;
            }

            //printf("\n%s (%d bits)\n",com->line, strlen(com->line));
        }
        com = com->next;
        currentAddress++;
    }
    return 1;
}

//Checks if the data is valid, and translate him.
char* checkData(char *line)
{
    char *aux;
    int value, valueSize;

    //Takes only the value
    aux = strchr(line, 'd');
    aux = strchr(aux, aux[1]);

    //Clean all spaces;
    while(aux[0] == ' ')
        aux = ++aux;

    //Converts the string to int
    valueSize = strlen(aux);
    value = atoi(aux);
    //If the value is wrong
    if(valueSize == 0 || (value == 0 && aux[0] != '0' && valueSize > 0))
        return "0";
	//Value overflow
    if(value > getBinaryRange(31, '+') || value < getBinaryRange(31, '-'))
    	return "1";

    //Converts to binary
    itoa(value, aux, 2);
    strncpy(aux,completeBinary(32, aux), 32);

    aux[32] = '\0';

    return aux;
}

//Checks if the instruction is valid
char* checkInstruction(char line[], int currentAddress)
{
    static char command[50], opcode[6];
    char data[16], function[6];
    static int nRegisters, nConstants, nLabels;
    char type;
    int i = 0;

    strcpy(command, line);

    //Take the mnemonic features
    strcpy(data, findInstruction(getMnemonic(command)));
    data[16] = '\0';

    //If the mnemonic not exists
    if(strcmp(data, "NULL") == 0)
        return "4";

    //Take the instruction type, n of regs, n of consts and n of labels.
    type = data[0];
    nRegisters = data[1] - 48;
    nConstants = data[2] - 48;
    nLabels = data[3] - 48;

    //Take the opcode
    for(i = 0; i<6; i++)
        opcode[i] = data[i+4];
    opcode[i] ='\0';

    if(type == 'R'){
        //Take the function
        for(i = 0; i<6; i++)
            function[i] = data[i+10];
        function[i] ='\0';
    }
    return mountBinary(command, nRegisters, nConstants, nLabels, type, opcode, function, currentAddress);
}

//Get the instruction mnemonic.
char* getMnemonic(char instruction[])
{
    static char mnemonic[10];
    int i = 0;
    while(instruction[i] != ' ' && instruction[i] != '\0' && instruction[i] != '\t'){
        mnemonic[i] = instruction[i];
        i++;
    }
    mnemonic[i] = '\0';

    return mnemonic;
}

//Completes the binary with a number of bits.
char* completeBinary(int numberOfBits, char binary[])
{
    //The nBits binary
    char newBinary[numberOfBits];
    //left 0's number
    int iMax = numberOfBits - strlen(binary);
    int i = 0;

    //Set the left 0's
    for(i = 0; i < iMax; i++)
        newBinary[i] = '0';

    //Concat and clean
    for(i = 1; i <= strlen(binary); i++)
        newBinary[numberOfBits - i] = binary[strlen(binary)-i];
    newBinary[numberOfBits] = '\0';

    return newBinary;
}

//Translate the instruction to binary
char* mountBinary(char instruction[],int nRegisters,int nConstants,int nLabels, char type, char *opcode, char *function, int currentAddress)
{
    int i = 0, j = 0, nRegsAux = 0, nConstAux = 0, nLabAux = 0;
    static char binary[32];
    char param[10], name[10];
    int isRegister = 0, isLabel = 0, isConstant = 0;

    //Define the initial binary
    for(i = 0; i < 32; i++)
        binary[i] = '0';
    i = 0;

    //Push the opcode
    concatBinary(0, 6, binary, opcode);
    
    //Takes the instruction name
    while(instruction[i] != ' '){
    	name[i] = instruction[i];
		i++;	
	}
    name[i] = '\0';
    i = 0;
    
    //Formats the instruction
    instruction = strchr(instruction, ' ');
    instruction = ++instruction;

    while(instruction[i] != '\0'){
        //Push the current instruction parameter
        if(instruction[i] != ','){
            param[j] = instruction[i];
            j++;
        }

        //If the char is comma or it is the instruction final
        if(instruction[i] == ',' || i == strlen(instruction) - 1){
            param[j] = '\0';
            
            //If the parameter is a register
            if(nRegisters != nRegsAux){
                isRegister = 1;
                isLabel = 0;
                isConstant = 0;
                nRegsAux++;
            }
            //If the parameter is a constant
            else if(nConstants != nConstAux){
                isRegister = 0;
                isLabel = 0;
                isConstant = 1;
                nConstAux++;
            }
            //If the parameter is a label
            else if(nLabels != nLabAux){
                isRegister = 0;
                isLabel = 1;
                isConstant = 0;
                nLabAux++;
            }

            //Operation for register
            if(isRegister == 1){
            	
            	//To load and store
            	char copy[10];
            	
            	//Load and Store
            	if(nRegsAux == 2 && (strcmp(name, "lw") == 0 || strcmp(name, "lh") == 0 || strcmp(name, "lb") == 0 || strcmp(name, "sw") == 0 || strcmp(name, "sh") == 0 || strcmp(name, "sb") == 0) )
				{ 
					strcpy(copy, param);
					
            		strcpy(param, strchr(param, '$'));
            		param[strlen(param) - 1] = '\0';   
					isRegister = 0;
                	isLabel = 0;
                	isConstant = 1;
               	    nConstAux++;	
				}
            	
                //Find the register code
                strcpy(param, findRegister(param));
                //If is not exists
                if(strcmp(param, "NULL") == 0)
                    return "1";

                //Push on correct position
                if(type == 'R'){
	                if(nRegsAux == 1){
						if(strcmp(name, "ext") == 0 || strcmp(name, "ins") == 0)
							concatBinary(11, 5, binary, param);
						else if(strcmp(name, "div") == 0 || strcmp(name, "divu") == 0 || strcmp(name, "madd") == 0  || strcmp(name, "maddu") == 0 || strcmp(name, "msub") == 0 || strcmp(name, "msubu") == 0  || strcmp(name, "mult") == 0  || strcmp(name, "multu") == 0 || strcmp(name, "mthi") == 0 || strcmp(name, "mtlo") == 0 || strcmp(name, "jr") == 0)
							concatBinary(6, 5, binary, param);
						else
	                		concatBinary(16, 5, binary, param);
	            	}
	                else if(nRegsAux == 2){
	                	if(strcmp(name, "wsbh") == 0 || strcmp(name, "seh") == 0 || strcmp(name, "seb") == 0 || strcmp(name, "div") == 0 || strcmp(name, "divu") == 0 || strcmp(name, "madd") == 0  || strcmp(name, "maddu") == 0 || strcmp(name, "msub") == 0 || strcmp(name, "msubu") == 0 || strcmp(name, "mult") == 0  || strcmp(name, "multu") == 0 || strcmp(name, "sll") == 0 || strcmp(name, "srl") == 0   || strcmp(name, "sra") == 0   || strcmp(name, "rotr") == 0  )
	                		concatBinary(11, 5, binary, param);
	                	else
	                		concatBinary(6, 5, binary, param);
					}
	                	
	                else if(nRegsAux == 3)
	                 	concatBinary(11, 5, binary, param);
	        	}
	        	else if(type == 'I'){
	        		if(nRegsAux == 1)
	                	concatBinary(11, 5, binary, param);
	                else if(nRegsAux == 2)
	                	concatBinary(6, 5, binary, param);
				}
                
                //To load and store
                if(isConstant == 1){
                	strcpy(param, copy);
                	strcpy(param, strchr(strrev(param), '('));
					strcpy(param, strrev(strchr(param, param[1])));
					while(param[0] == ' ' || param[0] == '\t')
						strcpy(param, strchr(param, param[1]));	
				}

            }
            //Operation for label
            else if(isLabel == 1){

                //Find the label code
                strcpy(param, findLabel(param));
                				
                //If is not valid
                if(strcmp(param, "NULL") == 0)
                    return "2";
                
				//Branchs instructions   
                if(strcmp(name, "beq") == 0 || strcmp(name, "bgtz") == 0 || strcmp(name, "bne") == 0 || strcmp(name, "bltz") == 0)
				{ 		
						int labelAddress = 0, i = 0;
						for(i; i < 16; i++){
							if(param[i] == '1')
								labelAddress++;
							if(i != 15)
								labelAddress = labelAddress << 1;
						}
						itoa(labelAddress - currentAddress, param, 2);
						strcpy(param, completeBinary(16, param));
						
				}

                //Push on correct position
                concatBinary(16, 16, binary, param);
            }
            //Operation for constant
            if(isConstant == 1){
            	//Convert to integer
               int constValue = atoi(param);
               
                //Verify if is valid
               if(constValue == 0 && param[0] != '0')
                    return "3";
               
			   if(type != 'R'){
			   		
			   		//Unsigned instructions
			   		if(strcmp(name, "addiu") == 0 || strcmp(name, "sltiu") == 0)
					{ 
						if(constValue < 0)
							return "7";
						if(constValue > getBinaryRange(17, '+'))
							return "6";

					}
					//Signed
					else{
				   	   if(constValue > getBinaryRange(16, '+') || constValue < getBinaryRange(16, '-'))
				   	  	 return "6";
		        	}
		        	 //Converts to binary and push to instruction
		            char constant[16];
		            itoa(constValue, constant, 2);
		            strcpy(constant, completeBinary(16, constant));
		            concatBinary(16, 16, binary, constant);
	           }
	           else{
	           	
	           	   if(constValue > getBinaryRange(5, '+') || constValue < getBinaryRange(5, '-'))
			   	  	 	return "6";

	           	   //Shamt, ins and ext
	               char constant[5];
	               int pos, size;
	               
	               if(strcmp(name, "ext") == 0 && nConstAux == 2)
				   		constValue--;
	               itoa(constValue, constant, 2);
	               strcpy(constant, completeBinary(5, constant));
	               
	               if(strcmp(name, "ext") == 0 && nConstAux == 1)
	               		concatBinary(21, 5, binary, constant);
	               else if(strcmp(name, "ext") == 0 && nConstAux == 2)
	               		concatBinary(16, 5, binary, constant);
	               else	if(strcmp(name, "ins") == 0 && nConstAux == 1)
	               		pos = constValue;
	                else if(strcmp(name, "ins") == 0 && nConstAux == 2){
	               		size = constValue;
	                	int msb = (pos + size) - 1;	               			
	               		itoa(msb, constant, 2);
	               		strcpy(constant, completeBinary(5, constant));
	               		concatBinary(16, 5, binary, constant);
	               		
	               		itoa(pos, constant, 2);
	               		strcpy(constant, completeBinary(5, constant));
	               		concatBinary(21, 5, binary, constant);
					}
	               else
	               		concatBinary(21, 5, binary, constant);
				   
			   }
            }
            param[0] = '\0';
            j = 0;
        }
        i++;
    }

    //Minus arguments
    if(nConstants != nConstAux || nRegisters != nRegsAux || nLabels != nLabAux)
        return "5";

    //Place the shamt and function on binary
    if(type == 'R'){
        concatBinary(26, 6, binary, function);
		
		//seh, seb and wsbh instructions
		if(strcmp(opcode, "011111") == 0 && strcmp(function, "100000") == 0){
			if(strcmp(name, "seh") == 0) 
				concatBinary(21, 5, binary, "11000");
			else if(strcmp(name, "seb") == 0) 
				concatBinary(21, 5, binary, "10000");
			else if(strcmp(name, "wsbh") == 0) 
				concatBinary(21, 5, binary, "00010");	
		}
		
		if(strcmp(name, "rotr") == 0)
			concatBinary(6, 5, binary, "00001");
		else if(strcmp(name, "rotrv") == 0)
			concatBinary(21, 5, binary, "00001");
	}

    binary[32] = '\0';
    return binary;
}

//Edit the instruction binary.
void concatBinary(int binaryIndex, int nBits, char binary[], char param[])
{
    int k = binaryIndex;
    int paramIndex = 0;

    for(binaryIndex; binaryIndex < (k+nBits); binaryIndex++){
        binary[binaryIndex] = param[paramIndex];
        paramIndex++;
    }
}

//Writes the binary file.
void writeOnFile()
{
    FILE *file;
	file = fopen("binary.txt", "w");
	char dRegBase[32], strAux[32];
	struct command *com, *aux;
    com = (struct command*)malloc(sizeof(struct command));
		
	//Puts the number of instruction and data in two first lines of the file
	itoa(numberOfInstructions, dRegBase, 2);
	strcpy(strAux, completeBinary(32, dRegBase));
	fputs(strAux, file);
	fputc('\n', file);
    com = first;
    while(com != NULL){
        fputs(com->line, file);
        com = com->next;
        if(com != NULL)
            fputc('\n', file);
    }
    fclose(file);
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
