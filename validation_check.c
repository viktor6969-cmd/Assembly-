#include "stdio.h"
#include <string.h>
#include "header.h"

/*------------DECLARATIONS--------------*/
int is_register(char* name);

/*--------------Functions---------------*/
int defenition_name_valid_check(char* name,label* label_head){

    label* label_temp;/*A pointer to run on all the linked label list*/
    label_temp = label_head;

    /*------Look for the name in lists---------*/
    while(label_temp!=NULL){

        if(strcmp(label_temp->name,name)==0)
            return 1;

        label_temp = label_temp->next;
    }

    /*---------Look in command arrey----------*/
    if(command_code(name)==NULL)
        return 2;

    /*---------Cheak if register name---------*/
    if(is_register(name))
        return 3;
        
    return 0;
}

int is_register(char* name){
   return((name[0]=='r')&&(name[1] >= '0' && name[1] <= '8'));
}

int isNumber(char *str) {
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

void print_error(char* error){
    printf("\x1b[31m%s \x1b[0m",error);
}
