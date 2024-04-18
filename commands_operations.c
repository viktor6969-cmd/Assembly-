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
char* red_error_str = "\x1b[31m ERROR\033[0m";
commands command_list[COMMAND_LIST_SIZE];
char binary_line[MAX_BINARY_LINE_SIZE];
char* file_name_global;
label* temp;
int comm_num = -1;
int origin_rows;

/*------------DECLARATIONS--------------*/
int write_operand(int type,int IC, char* first);
int operands_use_valid(int comm_num,int type1, int type2);
int command_line_check(char* line);
int command_number(char* name);
int opearnd_type(char* line);

/*--------------Functions---------------*/
int command_sort(char* name,char* line,int IC,int rows,char* file_name){
    char *first_operand = (char*)calloc(MAX_LINE_SIZE, sizeof(char));
    char *secnd_operand = (char*)calloc(MAX_LINE_SIZE, sizeof(char));
    int type1;
    int type2;
    int temp;

    *first_operand = '\0';
    *secnd_operand = '\0';
    file_name_global = file_name;
    origin_rows = rows;
    
    /*-------Check if command exist in command list------------*/
    if((comm_num = command_number(name)) < 0){ /*Return error if not*/
        printf("%s:%s:line %d: Uncnown command: '%s'\n",file_name,red_error_str,rows,name);
        return -1;
    }                   

    /*-----Check number of operands in line ----------*/
    switch(command_line_check(line)){

        /*---------The only operand is a name of a command 'hlt' or 'not'------*/
        case 0:
            sprintf(binary_line,"0%d\t0000%s000000",IC++,command_list[comm_num].binary_code);
            add_binary_line(binary_line,'h',1);
            return 1;

        /*------------There only one operand--------------------*/
        case 1:

        /*Comand validation check*/
        if(command_list[comm_num].operands!=1){
            printf("%s:%s at row %d: operands expected for '%s' command\n", file_name,red_error_str, rows, command_list[comm_num].name);
            return -1;
        }
        
        sscanf(line, "%s", first_operand);
        type1 = opearnd_type(first_operand);
        
        if(operands_use_valid(comm_num,type1,-1)){
                printf("%s:%s:line %d: invalid operads type for '%s' command\n",file_name,red_error_str,rows,command_list[comm_num].name);
                return -1;
            }

        if(type1 == 3){ /*Register sort*/
            sprintf(binary_line,"0%d\t0000%s00%s00",IC++,command_list[comm_num].binary_code,num_to_binary(type1,2));
            add_binary_line(binary_line,'c',1);/*Add the first line, with the sort number*/ 
            sprintf(binary_line,"0%d\t000000000%s00",IC++,string_to_binary(&first_operand[1],3));
            add_binary_line(binary_line,'r',1);/*Add the first line, with the sort number*/
            return 2;
        }
        sprintf(binary_line,"0%d\t0000%s00%s00",IC++,command_list[comm_num].binary_code,num_to_binary(opearnd_type(first_operand),2));
        add_binary_line(binary_line,'c',1);/*Add the first line, with the sort number*/
        temp = write_operand(opearnd_type(first_operand),IC,first_operand);/* Add the additional lines*/
        return temp + ((temp>0)?1:0);
    


        /*----------------Two operands------------------------*/
        case 2:
            sscanf(line, "%[^,],%s", first_operand, secnd_operand);
            type1 = opearnd_type(first_operand);
            type2 = opearnd_type(secnd_operand);

            if(operands_use_valid(comm_num,type1,type2)){
                printf("%s:%s:line %d: invalid operads type for '%s' command\n",file_name,red_error_str,rows,command_list[comm_num].name);
                return -1;
            }

        /*----------Check the valid operands amount-----------*/
            if(command_list[comm_num].operands!=2){
                printf("%s:%s:line %d: %d operand expected for '%s'\n", file_name,red_error_str, rows,command_list[comm_num].operands, command_list[comm_num].name);
                return -1;
            }

        /*--If both operands are registers put them in one line --*/
            if(type1 == 3 && type2 ==3){
                sprintf(binary_line,"0%d\t0000%s%s%s00",IC++,command_list[comm_num].binary_code,"11","11");
                add_binary_line(binary_line,'c',1);/*Add the first line, with both operand sotr numbers 11*/
                sprintf(binary_line,"0%d\t000000%s%s00",IC++,string_to_binary(&first_operand[1],3),string_to_binary(&secnd_operand[1],3));
                add_binary_line(binary_line,'r',1);/*Add the second line, with both registers numbers*/
                return 2;
            }
        
        /*----If only the first, add him , and then send the second one to 'write operand' function---*/
            if(type1 == 3){
                sprintf(binary_line,"0%d\t0000%s%s%s00",IC++,command_list[comm_num].binary_code,"11",num_to_binary(type2,2));
                add_binary_line(binary_line,'c',1);/*Add the first line, with the sort number*/
                sprintf(binary_line,"0%d\t000000%s00000",IC++,string_to_binary(&first_operand[1],3));
                add_binary_line(binary_line,'r',1);/*Add the first line, with the sort number*/
                temp = write_operand(type2,IC,secnd_operand);
                return (temp>0)?(temp+2):(-1);/* Add the additional*/
            }
        /*----Do the same but with second operand instead---*/
            if(type2 == 3){
                sprintf(binary_line,"0%d\t0000%s%s%s00",IC++,command_list[comm_num].binary_code,num_to_binary(type2,2),"11");
                add_binary_line(binary_line,'c',1);/*Add the first line, with the sort number*/
                sprintf(binary_line,"0%d\t000000000%s00",IC++,string_to_binary(&first_operand[1],3));
                add_binary_line(binary_line,'r',1);/*Add the first line, with the sort number*/
                temp = write_operand(type1,IC,first_operand);
                return (temp>0)?(temp+2):(-1);/* Add the additional*/
            }
    
        /*---If there no register sort, send both operands to 'write' function, and add sort numbers to the first command*/
            
            sprintf(binary_line,"0%d\t0000%s%s%s00",IC++,command_list[comm_num].binary_code,num_to_binary(type1,2),num_to_binary(type2,2));
            add_binary_line(binary_line,'c',1);/*Add the first line, with the sort number*/ 
            temp = (write_operand(type1,IC,first_operand));
            if(temp<0)
                return -1;
            IC += temp;
            type1 = write_operand(type2,IC,secnd_operand); /*Use type1 as second temp variable*/
            return (type1>0)?(1+ temp + type1):(-1); 
             
        /*If there more then 2 operands*/     
        case 3:
            printf("%s:%s:line %d:To many operands for \'%s\' command\n",file_name_global,red_error_str,origin_rows,command_list[comm_num].name);
            return 1;
    }
    
    return 0;
}

int write_operand(int type,int IC,char* first){
    char *temp_first = (char*)calloc(MAX_LABEL_NAME_SIZE, sizeof(char));
    char *temp_second = (char*)calloc(MAX_LABEL_NAME_SIZE, sizeof(char));
    int is_found;
    is_found = 0;
    switch(type){

        /*Immidiate sort*/
        case 0:
        /*If nuber add as binary line*/
        if((strlen(first)>=1) && is_number(&first[1])){
            sprintf(binary_line,"0%d\t%s00",IC++,string_to_binary(&first[1],12));
            add_binary_line(binary_line,'n',1);
            return 1;
        }
        /*Check if defined variable*/
        if((temp=in_data_list(&first[1],1))!=NULL){
            if((strcmp(temp->type,"mdefine")==0)){  
                sprintf(binary_line,"0%d\t%s00",IC,string_to_binary(temp->data,12));
                add_binary_line(binary_line,'D',1);
                return 1;
            }
            else{
                printf("%s:%s:line %d: Invalid usge of '%s' with the command '%s'\n",file_name_global,red_error_str,origin_rows,&first[1],command_list[comm_num].name);
                return -1;
            }
        }
        
        /*If undefined, print error and return -1*/
        printf("%s:%s:line %d:\'%s\' must be defined before use\n",file_name_global,red_error_str,origin_rows,&first[1]);
        return -1;
        
        /*-----------Direct sort---------------*/
        case 1:
        temp = in_data_list(first,0);
        if(temp != NULL){
            if(strcmp(temp->type,".extern")==0){
                sprintf(temp_first, "%d",IC);
                add_label(first,".extern_use",temp_first);
                sprintf(binary_line, "0%d\t00000000000001",IC);
                add_binary_line(binary_line,'e',1);
                return 1;
            }
        }
        /*If label not defined yet, put him as he is*/
        sprintf(binary_line,"0%d\t%s",IC++,first);
        add_binary_line(binary_line,'u',is_found);
        return 1;
        
        
        
        /*-----------ARRAY sort---------------*/
        case 2:
        sscanf(first, "%[^[][%[^]]]", temp_first, temp_second);
        temp=in_data_list(temp_first,2);

        /*If the Label name exist in Data list*/
        if(temp!=NULL){
            sprintf(temp_first,string_to_binary(temp->data,12));
            is_found = 1;
        }
        else
            is_found = 0;
        sprintf(binary_line,"0%d\t%s",IC++,temp_first);
        add_binary_line(binary_line,'u',is_found);/*Add the first line, with the sort number*/
        

        /*----Index check---------*/

        if(is_number(temp_second)){
            sprintf(binary_line,"0%d\t%s00",IC++,string_to_binary(temp_second,12));
            add_binary_line(binary_line,'i',1);
            return 2;
        }

        if(is_register(temp_second)){
            printf("%s:%s:line %d:\'%s\' register used as index in \'%s\'\n",file_name_global,red_error_str,origin_rows,temp_second,temp_first);
            return -1;
        }

        if(command_number(temp_second)>0){
            printf("%s:%s:line %d:\'%s\' command used as variable for \'%s\'\n",file_name_global,red_error_str,origin_rows,temp_second,temp_first);
            return -1;
        }
        /*-------Is label--------*/
        temp=in_data_list(temp_second,0);

        if(temp!=NULL){ /*if found, write the line*/
            sprintf(binary_line,"0%d\t%s00",IC++,string_to_binary(temp->data,12));
            add_binary_line(binary_line,'i',1);
            return 2;
        }
        /*If not foubd, write the name, and set the flag to 0*/
        sprintf(binary_line,"0%d\t%s",IC++,temp_second);
        add_binary_line(binary_line,'i',0);/*Add the first line, with the sort number*/
        return 2;
        
        /*If the label is undefined, put his name in the line*/
        case 4:
            sprintf(binary_line,"0%d\t%s",IC++,first);
            add_binary_line(binary_line,'u',0);/*Add the first line, with the sort number*/
            return 1;
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
    if (sscanf(name,"%[^[][%[^]]]",label,index) == 2) {
        return 2;
    }
    return 1; /*Undefined label*/
}

int make_command_list(){ /*Make space for new command list*/

    int i;
    char names[COMMAND_LIST_SIZE][5] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","hlt"};
    
    for(i = 0;i < COMMAND_LIST_SIZE; i++){
        /*---------Alocate memmory for the data-------------*/
        command_list[i].name = (char*)calloc(3, sizeof(char));; /* Max command length=3*/
        command_list[i].binary_code = (char*)calloc(4, sizeof(char));
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

int command_line_check(char* line){

    char first_op[MAX_LINE_SIZE];
    char secnd_op[MAX_LINE_SIZE];

    if(strcmp(line,"hlt")==0 || strcmp(line,"rts")==0)
        return 0;

    return sscanf(line, "%[^,],%s,%[^,]", first_op, secnd_op, line);
}

char* command_code(char* name){
    int i;
    char* binary;
    binary =(char*)calloc(4, sizeof(char));
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

int operands_use_valid(int comm_num,int type1, int type2){

        if(type2 == -1){ /*We have only one operand*/
            if(type1 == 0 && comm_num != 12)
                return 1;
            
            if(type1 == 2 &&(comm_num == 13 || comm_num == 10 || comm_num == 19))
                return 1;

            
        }
    return 0;
}