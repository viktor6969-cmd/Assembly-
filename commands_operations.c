#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

/*-------------STRUCTERS----------------*/
typedef struct commands{
    char* name;
    char* binary_code;
    int operands;
}commands;

/*----------GLOBAL VARIABLES------------*/
int find_operand(char* line,int num);
commands command_list[COMMAND_LIST_SIZE];
char binary_line[MAX_BINARY_LINE_SIZE];

/*------------DECLARATIONS--------------*/
int command_number(char* name);

/*--------------Functions---------------*/
int command_sort(char* name,char* line,int IC){
    int comm_num;
    int temp;
    char *frst_operand = malloc(2*(sizeof(char)));
    char *scnd_operand = malloc(2*(sizeof(char)));
    if((comm_num = command_number(name)) < 0) /*Return error if the command was not in the list*/
        return 0;
    sscanf(line, "%*s %[^\n]", line); /*Remuve the command name*/

    if(command_list[comm_num].operands==0){/*Its eather hlt or rts command*/
        sprintf(binary_line,"0%d\t0000%s000000",(IC+100),command_list[comm_num].binary_code);
        add_binary_line(binary_line,'c',1);
        return 1;/*1  line was added*/
    }

    if(command_list[comm_num].operands==1){
        switch(find_operand(line,comm_num)){

            case 1:
                sprintf(binary_line,"0%d\t0000%s000000",((IC++)+100),command_list[comm_num].binary_code);
                add_binary_line(binary_line,'c',1);
                sprintf(binary_line,"0%d\t%s00",((IC++)+100),string_to_binary(&line[1],12));
                add_binary_line(binary_line,'c',1);
                break;

        }

    }
    sscanf(line, "%s", frst_operand);
    sscanf(line, "%*s %s", scnd_operand);
    sprintf(binary_line,"0%d\t0000%s%s%s00",(IC+100),command_list[comm_num].binary_code, frst_operand,scnd_operand);
    add_binary_line(binary_line,'c',1);
    return 1;
}

int find_operand(char* name,int num){
    printf("the name:%s\n",name);
    if(name[0]=='#'){
        if(is_number(&name[1])){
            printf("the number is:%s\n",&name[1]);
            return 1;
        }
    }
    
    return 0;
}
 
int make_command_list(){ /*Make space for new command list*/

    int i;
    char names[COMMAND_LIST_SIZE][5] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","hlt"};
    
    for(i = 0;i < COMMAND_LIST_SIZE; i++){
        /*---------Alocate memmory for the data-------------*/
        command_list[i].name = malloc( 3 * sizeof(char)); /* Max command length=3*/
        command_list[i].binary_code = malloc(4 * sizeof(char));
        if (command_list[i].binary_code == NULL || command_list[i].name == NULL ) {
            print_error("Error allocating memory for command list");
            return -1;
        }
        /*--------------Fill up the data--------------------*/
        strcpy(command_list[i].name, names[i]);
        strcpy(command_list[i].binary_code,num_to_binary(i,4));
        if(i<=13)
            command_list[i].operands=(i<=8 ? 2:1);
        else 
            command_list[i].operands=0;
    }
    return 0;
}  

int command_number(char* name){
    int i;
    for(i=0;i<COMMAND_LIST_SIZE;i++){
        if(strcmp(name,command_list[i].name)==0)
            return i;
    }
    return -1;
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
    }
}