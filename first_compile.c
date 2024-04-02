#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define COMMAND_LIST_SIZE 16
#define MAX_LINE_SIZE 80
#define MAX_COMMAND_LINE_SIZE 14*3

/*-------------STRUCTERS----------------*/
typedef struct labels{
    char* name;
    struct labels* next;
}labels;

/*------------DECLARATIONS--------------*/
int is_prog(char* line,FILE* output_data_file);
int is_decloration(char* line, char* file_name);
int add_label(char* line);


/*----------GLOBAL VARIABLES------------*/
FILE* extern_file;
labels *labels_list_curent = NULL;
labels *labels_list_head = NULL;
labels *labels_temp;
char first_word[MAX_LINE_SIZE];
char second_word[MAX_LINE_SIZE];
char file_name_string[MAX_LINE_SIZE];
int IC;
int DC;


void first_read(char* input_file_name, int mcr){

    FILE *input_file;
    FILE *output_binary_file;
    char line[MAX_LINE_SIZE];
    char first_word[MAX_LINE_SIZE];
    IC = 0;
    DC = 0;

    if(mcr) /*Check if there is a macro file, if yes set the name to .am file*/
        sprintf(file_name_string,"%s.am",input_file_name);
    else /*If there is no macro file, set the name to the original file*/
        sprintf(file_name_string,"%s.as",input_file_name);

    input_file = fopen(file_name_string, "r"); /*Open the file*/

    sprintf(file_name_string,"%s.ob",input_file_name); /*Change the faile_name to file.ob*/
    output_binary_file = fopen(file_name_string, "w"); /*Open the output file to  write*/
    
    if (output_binary_file == NULL) /*Check if the file have been crieted*/
        print_error("Error creating object file");

    while (fgets(line, sizeof(line), input_file)){ /*Go thrue the file, line by line*/
        
        sscanf(line, "%s", first_word); /*Read the first word*/   
        if(line[0] == ';')/*Skip if it's a comment line*/
            continue;

        if(is_decloration(line,input_file_name))
            continue;   


        if((first_word[strlen(first_word)-1]) == ':') /*If the first word ends with ':' its a label*/
            /*add_label(line);*/
        
        fprintf(output_binary_file, "%d 0000\n", IC++);
    }
    
    fclose(output_binary_file);
}

int is_decloration(char* line, char* input_file_name){
    
    printf("%s\n",line);
    sscanf(line, "%s", first_word); /*take the first word of the line*/

    if((strcmp(first_word,".define")==0)){/*if its a define command*/
        sscanf(line, "%*[^ ] %[^=]", first_word);/*Scan the first word before the = char, (the variable name)*/
        sscanf(line, "%*[^=]=%s", second_word); /*Scan the first word after the = char, (the variable value)*/
        /*Add validation check*/
        return 1;
    }

    if(strcmp(first_word,".string")==0){
        return 1;
    }

    if(strcmp(first_word,".entry")==0){
        return 1;
    }

    if(strcmp(first_word,".extern")==0){
        printf("EXTERNAL ON %s",line);
        sprintf(file_name_string, "%s.ex", input_file_name);
        extern_file = fopen(file_name_string,"w");
        sscanf(line, "%*s %s", second_word);
        fprintf(extern_file, "%s\n",second_word);
        fclose(extern_file);
        return 1;
    }

    if(strcmp(first_word,".data")==0){
        return 1;
    }
    return 0;
}

int is_prog(char* line,FILE* output_data_file){
    return 0;
}

int add_label(char* line){

    sscanf(line, "%s", first_word);
    first_word[strlen(first_word) - 1] = '\0'; /*The label name, without the ':' char*/
    printf("The first word %s\n",first_word);
    labels_temp = labels_list_head;
    while (labels_temp != NULL) {
        if (strcmp(first_word, labels_temp->name) == 0) {
            /*print_error(first_word);*/
            return 1;
        }
        labels_temp = labels_temp->next;
    }

    labels_temp = malloc(sizeof(labels)); /*Allocate memory for the new label*/
    labels_temp->name = malloc(strlen(first_word) + 1); /*Allocate memory for the label name*/
    strcpy(labels_temp->name, first_word); /*Copy the label name*/
    labels_temp->next = labels_list_head; /*Set the next pointer to the current head*/
    labels_list_head = labels_temp; /*Set the new label as the head of the list*/

   /* fprintf(output_data_file, "%s %d\n", first_word, IC);*/ /*Add the label name, and the row to data output file*/
    return 0;
}
