#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

/*-------------STRUCTERS----------------*/
typedef struct commands{
    char* name;
    char* binary_code;
    char* operands;
}commands;

/*----------GLOBAL VARIABLES------------*/
commands command_list[COMMAND_LIST_SIZE];

/*--------------Functions---------------*/
int make_command_list(){ /*Make space for new command list*/

    int i;
    char names[COMMAND_LIST_SIZE][5] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","hlt"};
    
    for(i = 0;i < COMMAND_LIST_SIZE; i++){
        /*---------Alocate memmory for the data-------------*/
        command_list[i].name = malloc( 3 * sizeof(char)); /* Max command length=3*/
        command_list[i].binary_code = malloc(4 * sizeof(char));
        command_list[i].operands = malloc(2 * sizeof(char));
        if (command_list[i].binary_code == NULL || command_list[i].name == NULL || command_list[i].operands == NULL) {
            print_error("Error allocating memory for command list");
            return -1;
        }
        /*--------------Fill up the data--------------------*/
        strcpy(command_list[i].name, names[i]);
        strcpy(command_list[i].binary_code,num_to_binary(i,4));
        if(i<=13)
            strcpy(command_list[i].operands,(i<=8 ? "2":"1"));
        else 
            strcpy(command_list[i].operands,"0");
    }
    return 0;
}  

char* command_code(char* name){
    int i;
    char* binary;
    binary = malloc(4*sizeof(char));
    strcpy(binary,"");
    for(i=0;i<COMMAND_LIST_SIZE;i++)
        if(strcmp(name,command_list[i].name)==0)
            strcpy(binary,command_list[i].binary_code);
    return binary;
}

void free_command_list() { /*Clean the memmory alocated to command list */
    int i;
    for (i = 0; i < COMMAND_LIST_SIZE; i++) {
        if (command_list[i].name != NULL) 
            free(command_list[i].name);
        
        if (command_list[i].binary_code != NULL) 
            free(command_list[i].binary_code);
        
        if (command_list[i].operands != NULL) 
            free(command_list[i].operands);
    }
}