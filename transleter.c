#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define COMMAND_LIST_SIZE 16
#define MAX_LINE_SIZE 80
#define MAX_COMMAND_LINE_SIZE 14*3
#define FIRST_DIGITS "0000"

/*-------------STRUCTERS----------------*/
typedef struct commands{
    char* name;
    char* binary_code;
    char* operands;
}commands;

/*------------DECLARATIONS--------------*/
commands* make_command_list();
char* find_varable(char* variable_name);
void num_to_binary(char* num, char* binary_comand);
void imidiat_sort(char* binary_command, char* list);
void make_binary_comand(char* list,commands *command,char* com);


int is_defenition(char* line,char* command);
int is_label(char* line,char* command);
int is_command(char* line,char* command);
void find_operands(int num_of_oper,char* line, char* command);

/*----------GLOBAL VARIABLES------------*/
char first_word[MAX_LINE_SIZE];
char second_word[MAX_LINE_SIZE];
commands *command_list;
int row_counter;


/*-------------FUNCTIONS----------------*/
void sort_commands(FILE* input_file){
 
    FILE *output_file;
    char line[MAX_LINE_SIZE];
    char first_word[MAX_LINE_SIZE];
    char binary_command[MAX_COMMAND_LINE_SIZE];
    int (*functions[3])(char*,char*) = { is_defenition, is_command, is_label};
    int i;
    row_counter = 100;

    binary_command[0] = '\0'; /*Remuve random gabige*/
    command_list = make_command_list(); /*Make the command list from commands.txt*/
    output_file = fopen("binary.bin", "w"); /*Open the output file to  write*/
    if (output_file == NULL) {
        perror("Error creating binary.txt file");
    }

    while (fgets(line, sizeof(line), input_file)){ /*Go thrue the file, line by line*/
        
        sscanf(line, "%s", first_word); /*Read the first word*/   
        if(line[0] == ';')/*Skip if it's a comment line*/
            continue;

        for (i = 0; i < 3; i++) { /*Check the type of the command*/
            if (functions[i](line,binary_command)) {
                break; 
            }
        }

        if(binary_command[0]=='\0'){/*If this is not any valid command, send an error massage*/
            printf("\033[1;31m"); 
            printf("Invalid line: %s\n", line);
            printf("\033[0m"); 
        }

        else{
            fputs(binary_command,output_file);
            binary_command[0] = '\0';
        }
        i = 0;
    }
    
    fclose(output_file);
}

void compile_to_binary(FILE *input_file){
    
    FILE *output_file;
    char line[MAX_LINE_SIZE];
    char first_word[MAX_LINE_SIZE];
    char binary_command[MAX_COMMAND_LINE_SIZE];
    int i;
    commands *command_list;
    
    binary_command[0] = '\0'; /*To clean the garbige values*/ 
    command_list = make_command_list();
    output_file = fopen("binary.bin", "w");
    if (output_file == NULL) {
        perror("Error creating binary.txt file");
    }

    while (fgets(line, sizeof(line), input_file)){ /*Go thrue the file, line by line*/
        sscanf(line, "%s", first_word); /*Read the first word*/   
        if(strcmp(first_word,";")==0){
            continue;
        }

        for(i=0;i<COMMAND_LIST_SIZE;i++){
            if(strcmp(first_word,(command_list+i)->name)==0){
                sscanf(line, "%*s%[^\n]",line); /*Remove the first vord from the command string*/
                make_binary_comand(line,(command_list+i),binary_command);
                break;
            }
        }
        fputs(binary_command,output_file);
        binary_command[0] = '\0';
    }
}

commands* make_command_list(){ /*Make space for new command list*/
    int i;
    FILE *commands_file;
    char line[MAX_LINE_SIZE];
    commands* list = malloc(COMMAND_LIST_SIZE * sizeof(commands)); 
    if (list == NULL) {
        printf("Error allocating memory for command list");
        exit(EXIT_FAILURE);
    }
    commands_file = fopen("commands.cm", "r");
    if (commands_file == NULL) {
        printf("Error opening the commands.txt file");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < COMMAND_LIST_SIZE; i++) {

        fgets(line, sizeof(line), commands_file);
        list[i].binary_code = malloc(4 * sizeof(char));
        list[i].name = malloc(MAX_LINE_SIZE * sizeof(char));
        if (list[i].binary_code == NULL || list[i].name == NULL) {
            printf("Error allocating memory for binary code or name");
            exit(EXIT_FAILURE);
        }
        sscanf(line, "%s %s %s", list[i].binary_code, list[i].name, list[i].operands);
    }
    printf("name: %s  operants: %s", list[0].name, list[0].operands);
    fclose(commands_file);
    return list;
} 




void make_binary_comand(char* list,commands *command,char* binary_command){
    strcpy(binary_command,FIRST_DIGITS); /*Add the 0000 to the begining*/
    strcat(binary_command,command->binary_code); /*Add the OPCODE*/
    if(list[1] == '#'){ /*If the first operant is #, we have an ImidiateSort*/
        strcat(binary_command,"00"); /*Put 00 as the first operant*/
        imidiat_sort(binary_command,list+2); /*send the rest of the command, without the # char*/
    }
    strcat(binary_command,"\n");
}

void imidiat_sort(char* binary_command, char* list){

    char first_operand[MAX_LINE_SIZE];
    char second_operand[MAX_LINE_SIZE];
    printf("enter on %s\n",list);
    sscanf(list, "%[^,]",first_operand); /*The first operand after the # and before the ',' (if exist)*/
    sscanf(list, "%*[^,],%s",second_operand);/*The second operand, after the ','*/

    if(second_operand[0] == '\0') /*If there is only one operant*/
        strcat(binary_command,"0000\n"); /*Add the rest of the code, 00 for second operand, 00 for ARE, and a new line for the number*/
    else
        strcat(binary_command,"2200\n");

    if(isNumber(first_operand)) /*function exist in validationCheck.c file*/
            num_to_binary(first_operand,binary_command); /*if this is a number, put him as a binary line*/
        else
            num_to_binary(find_varable(first_operand),binary_command);/*if this is a variable, find his value*/
    
    if(isNumber(second_operand)) /*function exist in validationCheck.c file*/
            num_to_binary(second_operand,binary_command); /*if this is a number, put him as a binary line*/
        else
            num_to_binary(find_varable(second_operand),binary_command);/*if this is a variable, find his value*/
}
    
void num_to_binary(char* num, char* binary_command){
    strcat(binary_command,num);
}

char* find_varable(char* variable_name){
    return variable_name;
}

int is_defenition(char* line,char* command){
    sscanf(line, "%s", first_word); /*take the first word of the line*/
    sscanf(line, "%*s %s", second_word);/*Take the second word of the line*/

    if((strcmp(first_word,".define")==0)||(strcmp(second_word,".define")==0)){
        strcat(command,line);
        return 1;
    }

    if(strcmp(first_word,".string")==0){
        strcat(command,line);
        return 1;
    }

    if(strcmp(first_word,".entry")==0){
        strcat(command,line);
        return 1;
    }

    if(strcmp(first_word,".extern")==0){
        strcat(command,line);
        return 1;
    }
    return 0;
}

int is_label(char* line,char* command){

    return 1;
}

int is_command(char* line,char* command){

    /*sscanf(line, "%s", first_word); Read the first word*/   
    int i;
        for(i=0;i<COMMAND_LIST_SIZE;i++){
            if(strcmp(first_word,(command_list+i)->name)==0){
                strcat(command,"0000");
                strcat(command, (command_list+i)->binary_code);
               /* find_operands(i,line,command);*/
                return 1;
            }
        }
    return 0;
}

void find_operands(int i,char* line, char* command){
    strcat(command,"\n operands: ");
    strcat(command,(command_list+i)->operands);
}




void translater(){
    printf("Translator Done!");
}