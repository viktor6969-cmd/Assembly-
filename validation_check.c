#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "header.h"


/*------------DECLARATIONS--------------*/
int is_register(char* name);

char* red_error_mess = "\x1b[31m ERROR\033[0m";

/*--------------Functions---------------*/
int defenition_name_valid_check(char* name,label* label_head){

    label* label_temp;/*A pointer to run on all the linked label list*/
    label_temp = label_head;

    /*------Look for the name in lists---------*/
    while(label_temp!=NULL){

        if(strcmp(label_temp->name,name)==0){
            if(strcmp(label_temp->type,".extern")==0)
                return 5;
            if(strcmp(label_temp->type,".entry")==0)
                return 6;
            return 1;
        }
        label_temp = label_temp->next;
    }

    /*---------Look in command arrey----------*/
    if(command_code(name)==NULL)
        return 2;

    /*---------Cheak if register name---------*/
    if(is_register(name))
        return 3;

    if(is_number(name))
        return 4;
        
    return 0;
}

int operands_valid(char* line,int op_num){


    return 0;
}

int is_register(char* name){
   return((name[0]=='r')&&(name[1] >= '0' && name[1] <= '8')&&(strlen(name)==2));
}

int is_number(char *str) {
    if (str == NULL || *str == '\0') 
        return 0; 
    if (*str == '-') 
        str++; 
    while (*str != '\0') {
        if (*str < '0' || *str > '9') 
            return 0;        
        str++; 
    }
    return 1; 
}

int valid_data(char* word,char* file_name,int rows){

    char temp_str[MAX_BINARY_LINE_SIZE];
    label* temp_lbl;

    if(is_number(word)){ /*If it's a number add him*/
        sprintf(temp_str,"\t%s",num_to_binary(atoi(word),14));/*Take the string 'word', cust him to int, and convert to binary, then add to temp*/
        add_binary_line(temp_str,'d',0); /* Save in data list with row number*/
        strcpy(word,"");
        return 1;
    }
    if((temp_lbl=in_data_list(word,1))!=NULL){ /*Check the labels list*/
        if(strcmp(temp_lbl->type,".extern")==0){
            printf("%s:%s:line %d:Can't use an extern variable '%s' as data input: \n",file_name,red_error_mess,rows,word);
            return -1;
            }
        sprintf(temp_str,"\t%s",num_to_binary(atoi(in_data_list(word,1)->data),14));
        add_binary_line(temp_str,'d',0);
        strcpy(word,"");
        return 1;
        }

    else{
        printf("%s:%s:line %d:Invalid data input:'%s' \n",file_name,red_error_mess,rows,word);
        return 1;
    }
    return 0;
}

