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
int opearnd_type(char* line);
commands command_list[COMMAND_LIST_SIZE];
char binary_line[MAX_BINARY_LINE_SIZE];
label* temp;
int comm_num = -1;
int origin_rows;

/*------------DECLARATIONS--------------*/
int write_operand(int type,int IC, char* first);
int command_number(char* name);

/*--------------Functions---------------*/
int command_sort(char* name,char* line,int IC,int rows){
    char *first_operand = malloc(MAX_LINE_SIZE*(sizeof(char)));
    char *secnd_operand = malloc(MAX_LINE_SIZE*(sizeof(char)));
    int type1;
    int type2;
    *first_operand = '\0';
    *secnd_operand = '\0';
    origin_rows = rows;
    if((comm_num = command_number(name)) < 0){ /*Return error if the command was not in the list*/
        printf("\x1b[31mError in line %d: Uncnown command: \'%s\'\x1b[0m\n",rows,command_list[comm_num].name);
        return -1;
    }
    /*-----------The are no variables------------*/
    if(strcmp(name,line)==0){
        if((strcmp(name,"hlt")==0)||(strcmp(name,"not")==0)){
            sprintf(binary_line,"0%d\t0000%s000000",IC++,command_list[comm_num].binary_code);
            add_binary_line(binary_line,'Y',1);
            return 1;
        }
    }

    /*------------Two variables-------------------*/
    if(sscanf(line, "%[^,],%s", first_operand, secnd_operand)==2){
        type1 = opearnd_type(first_operand);
        type2 = opearnd_type(secnd_operand);

        if(type1 == 3 && type2 ==3){
            sprintf(binary_line,"0%d\t0000%s%s%s00",IC++,command_list[comm_num].binary_code,"11","11");
            add_binary_line(binary_line,'r',1);/*Add the first line, with the sort number*/
            sprintf(binary_line,"0%d\t000000%s%s00",IC++,string_to_binary(&first_operand[1],3),string_to_binary(&secnd_operand[1],3));
            add_binary_line(binary_line,'R',1);/*Add the first line, with the sort number*/
            return 2;
        }
        
        if(type1 == 3){
            sprintf(binary_line,"0%d\t0000%s%s%s00",IC++,command_list[comm_num].binary_code,"11",num_to_binary(type2,2));
            add_binary_line(binary_line,'r',1);/*Add the first line, with the sort number*/
            sprintf(binary_line,"0%d\t000000%s00000",IC++,string_to_binary(&first_operand[1],3));
            add_binary_line(binary_line,'R',1);/*Add the first line, with the sort number*/
            return 2+(write_operand(type1,IC,first_operand));/* Add the additional*/
        }

        if(type2 == 3){
            sprintf(binary_line,"0%d\t0000%s%s%s00",IC++,command_list[comm_num].binary_code,num_to_binary(type2,2),"11");
            add_binary_line(binary_line,'r',1);/*Add the first line, with the sort number*/
            sprintf(binary_line,"0%d\t000000000%s00",IC++,string_to_binary(&first_operand[1],3));
            add_binary_line(binary_line,'R',1);/*Add the first line, with the sort number*/
            return 2+(write_operand(type1,IC,first_operand));/* Add the additional*/
        }
        
        sprintf(binary_line,"0%d\t0000%s%s%s00",IC++,command_list[comm_num].binary_code,num_to_binary(type1,2),num_to_binary(type2,2));
        add_binary_line(binary_line,'c',1);/*Add the first line, with the sort number*/       
        write_operand(type1,IC,first_operand);/* Add the additional*/
        write_operand(type2,IC,secnd_operand);/* Add the additional*/
        return 2;
    }
    /*------------One variable--------------------*/
    else{
        sprintf(binary_line,"0%d\t0000%s00%s00",IC++,command_list[comm_num].binary_code,num_to_binary(opearnd_type(first_operand),2));
        add_binary_line(binary_line,'c',1);/*Add the first line, with the sort number*/
        write_operand(opearnd_type(first_operand),IC,first_operand);/* Add the additional lines*/
        return 2;
    }
    
    return 0;
}

int write_operand(int type,int IC,char* first){
    char *temp_first = malloc(MAX_LABEL_NAME_SIZE*sizeof(char));
    char *temp_second = malloc(MAX_LABEL_NAME_SIZE*sizeof(char));
    int is_found;
    switch(type){

        case 0:/*Immidiate sort*/
        if((temp=in_data_list(&first[1]))!=NULL){  
            if(strcmp(temp->type,"mdefine")==0){
               
                sprintf(binary_line,"0%d\t%s00",IC,string_to_binary(temp->data,12));
                add_binary_line(binary_line,'L',1);
                return 1;
            }
            
            printf("\x1b[31mError in line %d:Can not use: \'%s\' as variable for \'%s\'\x1b[0m\n",origin_rows,&first[1],command_list[comm_num].name);
            return 1;
        }
        
        if((strlen(first)>=1) && is_number(&first[1])){
            sprintf(binary_line,"0%d\t%s00",IC++,string_to_binary(&first[1],12));
            add_binary_line(binary_line,'n',1);
            return 1;
        }
        return -1;
        
        /*-----------Direct sort---------------*/
        case 1:
        temp = in_data_list(first);
        if(temp != NULL && strcmp(temp->type,".extern")==0){
            sprintf(temp_first, "%d",IC);
            add_label(first,".extern_use",temp_first);
            sprintf(binary_line, "0%d\t00000000000001",IC);
            add_binary_line(binary_line,'E',1);
        }
        return 1;
        
        /*-----------ARRAY sort---------------*/
        case 2:
        printf("The first:%s\n",first);
        sscanf(first, "%[^[][%[^]]]", temp_first, temp_second);
        temp=in_data_list(temp_first);
        /*If the name exist in Data list*/
        if(temp!=NULL){
            printf("In if: first \'%s\'\n",temp_first);
            sprintf(temp_first,string_to_binary(temp->data,12));
            is_found = 1;
        }
        else
            is_found = 0;

        sprintf(binary_line,"0%d\t%s00",IC++,temp_first);
        printf("Before write: first \'%s\'\n",temp_first);
        add_binary_line(binary_line,'A',is_found);/*Add the first line, with the sort number*/
        
        temp=in_data_list(temp_second);
        if(temp!=NULL){
            sprintf(temp_second,string_to_binary(temp->data,12));
            is_found = 1;
        }
        else if(is_number(temp_second)){
                sprintf(temp_second,string_to_binary(temp_second,12));
                is_found = 1;
                }
            else
                is_found = 0;
        
        sprintf(binary_line,"0%d\t%s00",IC++,temp_second);
        add_binary_line(binary_line,'a',is_found);/*Add the first line, with the sort number*/
        return 2;
        
        /*-------------Register sort-----------*/
        case 3:
        


    }
    return 0;
}

int opearnd_type (char* name){
    char label[MAX_LABEL_NAME_SIZE];
    char index[MAX_LABEL_NAME_SIZE];
    /*printf("Find operand:%s\n",name);*/
    if(name[0]=='#')
        return 0;
    if(is_register(name))
        return 3;
    if((temp=in_data_list(name))!=NULL){/*The operand is a label*/
        return 1;
    }
    if (sscanf(name,"%[^[][%[^]]]",label,index) == 2) {
        return 2;
    }
    return 1;
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
            command_list[i].operands=((i<4 || i == 6) ? 2:1);
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