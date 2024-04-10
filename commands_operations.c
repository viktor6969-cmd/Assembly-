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
label* temp;

/*------------DECLARATIONS--------------*/
int command_number(char* name);

/*--------------Functions---------------*/
int command_sort(char* name,char* line,int IC,int rows){
    int comm_num;
    char *frst_operand = malloc(MAX_LINE_SIZE*(sizeof(char)));
    char *scnd_operand = malloc(MAX_LINE_SIZE*(sizeof(char)));
    *frst_operand = '\0';
    if((comm_num = command_number(name)) < 0) /*Return error if the command was not in the list*/
        return 0;
    sscanf(line, "%*s %[^\n]", line); /*Remuve the command name*/

    if(command_list[comm_num].operands==0){/*Its eather hlt or rts command*/
        sprintf(binary_line,"0%d\t0000%s000000",IC,command_list[comm_num].binary_code);
        add_binary_line(binary_line,'c',1);
        return 1;/*1  line was added*/
    }

    if(command_list[comm_num].operands==1){
        sscanf(line, "%[^, \t]", frst_operand);
        if(strcmp(line,frst_operand)!=0){
            printf("\x1b[31mError in line %d: Too many arguments for \'%s\'\x1b[0m\n",rows,command_list[comm_num].name);
            return -1;
        }
        switch(find_operand(line,comm_num)){
            
            /*-------------------DYRECT SORT----------------------*/

            case 1: /*The operand is dyrect sort, '#number' type*/
                sprintf(binary_line,"0%d\t0000%s000000",IC++,command_list[comm_num].binary_code);
                add_binary_line(binary_line,'c',1);
                sprintf(binary_line,"0%d\t%s00",IC++,string_to_binary(&line[1],12));
                add_binary_line(binary_line,'c',1);
                return 2;
                break;
            
            case 11:/*The operand is dyrect sort, defined variable type*/
                sprintf(binary_line,"0%d\t0000%s000000",IC++,command_list[comm_num].binary_code);
                add_binary_line(binary_line,'c',1);
                sprintf(binary_line,"0%d\t%s00",IC++,string_to_binary(temp->data,12));
                add_binary_line(binary_line,'c',1);
                return 2;

            case -1:
                printf("\x1b[31mError in line %d: Undeclered variable :\'%s\'\x1b[0m\n",rows,&line[1]);
        
            /*-------------------Direct sort---------------------*/

            case 2:
                
                /* If the variable is external*/
                if(strcmp(temp->type,".extern")==0){ 
                    sprintf(binary_line,"0%d\t0000%s000100",IC++,command_list[comm_num].binary_code);
                    add_binary_line(binary_line,'c',1);
                    sprintf(binary_line,"0%d\t00000000000001",IC);
                    add_binary_line(binary_line,'x',1);/*Put the line in the list, vith type 'extern' and the name of the variable*/
                    sprintf(binary_line,"0%d",IC);
                    add_label(line,".ext_call",binary_line);
                    return 2;
                }
                
                sprintf(binary_line,"0%d\t0000%s000100",IC++,command_list[comm_num].binary_code);
                add_binary_line(binary_line,'c',1);
                sprintf(binary_line,"0%d\t%s00",IC,string_to_binary(temp->data,12));
                add_binary_line(binary_line,'l',1);
                return 2;
            
            case -2:

                printf("\x1b[31mError in line %d: Invalid use of:\'%s\'\x1b[0m\n",rows,line);
                
            /*-------------------Constant index sort----------------*/
            case 3:
                sscanf(line,"%[^[][%[^]]]",frst_operand,scnd_operand);
                printf("Name:%s labeel:%s index:%s\n",name,frst_operand,scnd_operand);    
                sprintf(binary_line,"0%d\t0000%s001000",IC++,command_list[comm_num].binary_code);
                add_binary_line(binary_line,'a',1);
                temp = in_data_list(frst_operand);/*To make shure that temp is on the label name*/
                sprintf(binary_line,"0%d\t%s00",IC,string_to_binary(temp->data,12));
                add_binary_line(binary_line,'A',1);
                sprintf(binary_line,"0%d\t%s00",IC,string_to_binary(scnd_operand,12));
                add_binary_line(binary_line,'I',1);
                return 3;

            case 31:
                sscanf(line,"%[^[][%[^]]]",frst_operand,scnd_operand);
                printf("Name22:%s, first:%s index:%s\n",name,frst_operand,scnd_operand);
                sprintf(binary_line,"0%d\t0000%s001000",IC++,command_list[comm_num].binary_code);

                printf("Line:%s\n",binary_line);
                add_binary_line(binary_line,'c',1);
                temp = in_data_list(frst_operand);/*To make shure that temp is on the label name*/
                sprintf(binary_line,"0%d\t%s00",IC,string_to_binary(temp->data,12));
                add_binary_line(binary_line,'A',1);
                temp = in_data_list(scnd_operand);
                sprintf(binary_line,"0%d\t%s00",IC,string_to_binary(temp->data,12));
                add_binary_line(binary_line,'I',1);
                return 3;
            case -31:

                return 3;
            
            case -32:
                printf("\x1b[31mError in line %d: The array index:\'%s\' is invalid\x1b[0m\n",rows,scnd_operand);
                return 0;
            /*----------------------Register sort-------------------*/
            case 4:/*The operand is a register*/

                sprintf(binary_line,"0%d\t0000%s00110000",IC++,command_list[comm_num].binary_code);
                add_binary_line(binary_line,'c',1);
                /*FIIIIIX THE REGISTER WRIGTH*/
                sprintf(binary_line,"0%d\t0000000000%s00",IC++,string_to_binary(&line[1],2));
                add_binary_line(binary_line,'c',0);
                return 2;
        }

    }
    sscanf(line, "%s", frst_operand);
    sscanf(line, "%*s %s", scnd_operand);
    sprintf(binary_line,"0%d\t0000%s%s%s00",IC++,command_list[comm_num].binary_code, frst_operand,scnd_operand);
    add_binary_line(binary_line,'c',1);
    free(frst_operand);
    free(scnd_operand);
    return 1;
}

int find_operand(char* name,int num){
    
    char label[MAX_LABEL_NAME_SIZE];
    char index[MAX_LABEL_NAME_SIZE];
    /*printf("Find operand:%s\n",name);*/
    
    if(name[0]=='#'){
        if(is_number(&name[1]))
            return 1; /*The operand is a valid number*/
        if(in_data_list(&name[1]))
            return 11;

        /*If not in list, this variable is undeclined*/
        return -1; 
    }

    /*check is register*/
    if(is_register(name))
        return 4;  

    /*Check if existing label*/
    if((temp=in_data_list(name))!=NULL){/*The operand is a label*/
            if((strcmp(temp->type,".entry")!=0) && (strcmp(temp->type,"mdefine")!=0))
                return 2; /*The operand is data type label*/
            else
                return -2; /*The sort */
    }

    if (sscanf(name,"%[^[][%[^]]]",label,index) == 2) {
        printf("valid array:%s, with index %s\n",label,index);
        if((in_data_list(label))&&(is_number(index)||in_data_list(index))){
            if(is_number(index))
                return 3;
            if(in_data_list(index)&&(strcmp(temp->type,"mdefine")==0))
                return 31;
            else{
                return -31;
            }
        }
        /*If the label name or the index is not a number or not in data list, they undefined yet*/
        return -32;
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