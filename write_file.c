#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

/*------------DECLARATIONS--------------*/

/*----------GLOBAL VARIABLES------------*/


int printf_binary_files(binary *binary_list,label *label_list,char* file_name){

    char* string_temp = malloc(MAX_LABEL_NAME_SIZE*sizeof(char));
    binary* binary_temp = binary_list;
    label* label_temp = label_list;
    FILE* binary_output = NULL;
    FILE* extern_output = NULL;
    FILE* entry_output = NULL;

    sprintf(string_temp,"%s.ob",file_name);
    binary_output = fopen(string_temp,"w");
    if(binary_output==NULL){
        printf("ERROR: faild to create %s.ob file\n",file_name);
        return -1;
    }

    while(binary_temp!=NULL){
        fprintf(binary_output,"%s\n",binary_temp->data);
        binary_temp = binary_temp->next;
    }

    while (label_temp!=NULL)
    {
        if(strcmp(label_temp->type,".extern_use")==0){
            sprintf(string_temp,"%s.ext",file_name);
            extern_output = fopen(string_temp,"a");
            fprintf(extern_output,"%s\t%s\n",label_temp->name,label_temp->data);
            fclose(extern_output);
        }
        if(strcmp(label_temp->type,".entry_use")==0){
            sprintf(string_temp,"%s.ent",file_name);
            entry_output = fopen(string_temp,"a");
            fprintf(entry_output,"%s\t%s\n",label_temp->name,label_temp->data);
            fclose(entry_output);
        }
        label_temp = label_temp->next;
    }
    
    fclose(binary_output);
    return 0;
}