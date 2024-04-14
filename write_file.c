#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

/*------------DECLARATIONS--------------*/
char* translate(char* line);
char* weird_code(char* base4);

/*-------------FUNCTIONS----------------*/
int printf_binary_files(binary *binary_list,label *label_list,char* file_name){

    char* string_temp = malloc(MAX_LABEL_NAME_SIZE*sizeof(char));
    char* line = malloc(4*sizeof(char));
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
        sscanf(binary_temp->data,"%s %s",line,string_temp);
        
        fprintf(binary_output,"%s\t%s\n",line,translate(string_temp));
        binary_temp = binary_temp->next;
    }

    while (label_temp!=NULL)
    {
        if(strcmp(label_temp->type,".extern_use")==0){
            sprintf(string_temp,"%s.ext",file_name);
            extern_output = fopen(string_temp,"a");
            fprintf(extern_output,"%s\t0%s\n",label_temp->name,label_temp->data);
            fclose(extern_output);
        }
        if(strcmp(label_temp->type,".entry_use")==0){
            sprintf(string_temp,"%s.ent",file_name);
            entry_output = fopen(string_temp,"a");
            fprintf(entry_output,"%s\t0%s\n",label_temp->name,label_temp->data);
            fclose(entry_output);
        }
        label_temp = label_temp->next;
    }
    
    fclose(binary_output);
    return 0;
}

char* translate(char* binary) {

    int i;
    int val;
    int base4_len = 7; 
    char* base4 = (char*)calloc(14, sizeof(char));
    char lookup[] = {'0', '1', '2', '3'};
    if (!base4) {
        printf("Memory allocation failed\n");
        return NULL;
    }
 
    for (i = 0; i < base4_len; i++) {
        val = (binary[2*i] - '0') * 2 + (binary[2*i + 1] - '0');
        base4[i] = lookup[val];
    }
    base4[base4_len] = '\0'; 
    return weird_code(base4);
}

char* weird_code(char* base4) {
    int i;
    int digit;
    int base4_len = strlen(base4);
    char* weird = (char*)malloc(base4_len + 1); 
    char mapping[] = {'*', '#', '%', '!'};
    if (!weird) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    
    for (i = 0; i < base4_len; i++) {
        digit = base4[i] - '0';
        if (digit < 0 || digit > 3) {
            free(weird);
            printf("Invalid base-4 input: %c\n", base4[i]);
            return NULL;
        }
        weird[i] = mapping[digit];
    }
    weird[base4_len] = '\0'; 
    return weird;
}