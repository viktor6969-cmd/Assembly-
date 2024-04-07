#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define COMMAND_LIST_SIZE 16
#define MAX_LINE_SIZE 81
#define MAX_COMMAND_LINE_SIZE 14*3

/*-------------STRUCTERS----------------*/
typedef struct commands{
    char* name;
    char* binary_code;
    char* operands;
}commands;

/*------------DECLARATIONS--------------*/

/*-------------VARIABLES----------------*/

commands list[COMMAND_LIST_SIZE];

int i;
/*------------FUNCTIONSS----------------*/
int command_data_write(char* line,int IC,FILE* output_file){ /*will return the number of lines for this command*/
    char first_word[MAX_LINE_SIZE];
    sscanf(line, "%s", first_word);
    for(i=0;i<COMMAND_LIST_SIZE;i++){
        if(strcmp(command_list[i].name,first_word)==0){
            fprintf(output_file, "%d\t0000%s", IC++,command_list[i].binary_code);
            sscanf(line, "%*s %[^\n]", line); /*Remuve the name of the command*/


            return 1;
        }
    }
    printf("ERROR:The line: %s is invalid",line);
    return -1;
}

int make_command_list(){ /*Make space for new command list*/
    int i;
    char names[COMMAND_LIST_SIZE][5] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","hlt"};
    for(i = 0;i < COMMAND_LIST_SIZE; i++){

        /*---------Alocate memmory for the data-------------*/
        list[i].name = malloc(MAX_LINE_SIZE * sizeof(char));
        list[i].binary_code = malloc(4 * sizeof(char));
        list[i].operands = malloc(2 * sizeof(char));
        if (list[i].binary_code == NULL || list[i].name == NULL || list[i].operands == NULL) {
            print_error("Error allocating memory for command list");
            return -1;
        }
        /*--------------Fill up the data--------------------*/
        sscanf(list[i].name,names[i]);
        sscanf(list[i].binary_code,num_to_binary(i,4));

        
    }
    command_list = list;
    return 0;
} 

void free_command_list() {
    int i;
    if (command_list == NULL) {
        return;
    }

    for (i = 0; i < COMMAND_LIST_SIZE; i++) {
        free(command_list[i].name);
        free(command_list[i].binary_code);
        free(command_list[i].operands);
    }

    free(command_list);
    command_list = NULL;
}
