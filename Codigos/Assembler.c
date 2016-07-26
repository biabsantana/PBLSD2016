#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "uthash.h"

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
char* checkInstruction(char line[]);
char* getMnemonic(char instruction[]);
char* completeBinary(int numberOfBits, char binary[]);
char* mountBinary(char instruction[],int nRegisters,int nConstants,int nLabels, char type, char *opcode, char *function);
void concatBinary(int binaryIndex, int nBits, char binary[], char param[]);
void writeOnFile();
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
                    printf("\n-------------------------------------------------------------------------------------------------------------------");
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

    printf("---------------------------------TEC499 - ASSEMBLER---------------------------------\n\n");

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
    struct hash *labl;
    //Verifies if the register already exists on hash
    HASH_FIND_STR(labels, key, labl);

    //If not exists, creates.
    if(labl == NULL)
    {
        int i = 0;
        while(key[i] == ' ')
            key = ++key;

        labl = (struct hash*)malloc(sizeof(struct hash));
        strncpy(labl->key, key, 100);
        labl->value = value;
        HASH_ADD_STR(labels, key, labl);
    }
}

//Get the label code
char* findLabel(char key[])
{
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
    addInstruction("add", "R300000000000000");
    addInstruction("sub", "R300000000000001");
    addInstruction("mul", "R300000000000010");
    addInstruction("li", "I110000001");
    addInstruction("beq", "I201000010");
    addInstruction("j", "J001000011");
    addInstruction("sw", "I101000100");

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

}

//Verifies all instructions and data on list and translate to binary
int verifyAndTranslate()
{
    struct command *com, *aux;
    char result[32];

    com = (struct command*)malloc(sizeof(struct command));
    com = first;
    while(com != NULL){
        //Directive
        if(getDirective(com->line) == 3){
            //printf("\n\nDado: (%s)",com->line);
            strcpy(com->line, checkData(com->line));
            if(strcmp(com->line, "0") == 0){
                printf("\n-> ERROR(9): INVALID VARIABLE - Line %d", com->lineNumber);
                return 0;
            }
            //printf("\n%s (%d bits)\n",com->line, strlen(com->line));
        }
        //Instruction
        else{
            //printf("\n\nInstrucao: (%s)",com->line);
            strcpy(com->line, checkInstruction(com->line));
            if(strcmp(com->line, "1") == 0){
                printf("\n-------------------------------------------------------------------------------------------------------------------");
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(6): INVALID REGISTER - Line %d", com->lineNumber);
                return 0;
            }
            else if(strcmp(com->line, "2") == 0){
                printf("\n-------------------------------------------------------------------------------------------------------------------");
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(7): INVALID LABEL - Line %d", com->lineNumber);
                return 0;
            }
            else if(strcmp(com->line, "3") == 0){
                printf("\n-------------------------------------------------------------------------------------------------------------------");
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(8): INVALID CONSTANT - Line %d", com->lineNumber);
                return 0;
            }
            else if(strcmp(com->line, "4") == 0){
                printf("\n-------------------------------------------------------------------------------------------------------------------");
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(10): INVALID MNEMONIC - Line %d", com->lineNumber);
                return 0;
            }

            else if(strcmp(com->line, "5") == 0){
                printf("\n-------------------------------------------------------------------------------------------------------------------");
                printf("\nPROGRAM NOT ASSEMBLED");
                printf("\n-> ERROR(11): MISSING PARAMETERS - Line %d", com->lineNumber);
                return 0;
            }

            //printf("\n%s (%d bits)\n",com->line, strlen(com->line));
        }
        com = com->next;
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

    //Converts to binary
    itoa(value, aux, 2);
    strncpy(aux,completeBinary(32, aux), 32);

    //Define the opcode 111111
    for(value = 0; value < 6; value++)
        aux[value] = '1';
    aux[32] = '\0';

    return aux;
}

//Checks if the instruction is valid
char* checkInstruction(char line[])
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
    return mountBinary(command, nRegisters, nConstants, nLabels, type, opcode, function);
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
char* mountBinary(char instruction[],int nRegisters,int nConstants,int nLabels, char type, char *opcode, char *function)
{
    int i = 0, j = 0, nRegsAux = 0, nConstAux = 0, nLabAux = 0;
    static char binary[32];
    char param[10];
    int isRegister = 0, isLabel = 0, isConstant = 0;

    //Define the initial binary
    for(i = 0; i < 32; i++)
        binary[i] = '0';
    i = 0;

    //Push the opcode
    concatBinary(0, 6, binary, opcode);
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
                //Find the register code
                strcpy(param, findRegister(param));
                //If is not exists
                if(strcmp(param, "NULL") == 0)
                    return "1";

                //Push on correct position
                if(nRegsAux == 1)
                    concatBinary(6, 5, binary, param);
                else if(nRegsAux == 2)
                    concatBinary(11, 5, binary, param);
                else if(nRegsAux == 3)
                    concatBinary(16, 5, binary, param);

            }
            //Operation for label
            else if(isLabel == 1){
                //Find the label code
                strcpy(param, findLabel(param));
                //If is not valid
                if(strcmp(param, "NULL") == 0)
                    return "2";

                //Push on correct position
                concatBinary(16, 16, binary, param);
            }
            //Operation for constant
            else if(isConstant == 1){
                //Convert to integer
               int constValue = atoi(param);
                //Verify if is valid
               if(constValue == 0 && param[0] != 0)
                    return "3";
                //Converts to binary and push to instruction
               char constant[16];
               itoa(constValue, constant, 2);
               strcpy(constant, completeBinary(16, constant));
               concatBinary(16, 16, binary, constant);
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
        concatBinary(21, 5, binary, "00000");
        concatBinary(26, 5, binary, function);
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

	struct command *com, *aux;
    com = (struct command*)malloc(sizeof(struct command));

    com = first;
    while(com != NULL){
        fputs(com->line, file);
        fputc('\n', file);
        com = com->next;
    }
    fclose(file);
}

