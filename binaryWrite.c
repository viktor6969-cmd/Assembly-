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
int is_decloration(char* line,FILE* output_file);
int add_label(char* line,FILE* output_data_file);


/*----------GLOBAL VARIABLES------------*/
labels *labels_list_curent = NULL;
labels *labels_list_head = NULL;
labels *labels_temp;
char first_word[MAX_LINE_SIZE];
char second_word[MAX_LINE_SIZE];
int row_counter;


void first_read(FILE* input_file){
    FILE *output_binary_file;
    FILE *output_data_file;
    char line[MAX_LINE_SIZE];
    char first_word[MAX_LINE_SIZE];
    /*char binary_command[MAX_COMMAND_LINE_SIZE];*/
    row_counter = 100;

    output_binary_file = fopen("binary.bin", "w"); /*Open the output file to  write*/
    if (output_binary_file == NULL) {
        perror("Error creating binary.txt file");
    }

    output_data_file = fopen("data.bin", "w"); /*Open the output file to  write*/
    if (output_data_file == NULL) {
        perror("Error creating table file");
    }

    while (fgets(line, sizeof(line), input_file)){ /*Go thrue the file, line by line*/
        
        sscanf(line, "%s", first_word); /*Read the first word*/   
        if(line[0] == ';')/*Skip if it's a comment line*/
            continue;

        if((first_word[strlen(first_word)-1]) == ':'){ /*If the first word ends with ':' its a label*/
 
            add_label(line,output_data_file);
        }

        if(is_decloration(line,output_data_file))
            continue;   

        fprintf(output_binary_file, "%04d 0000\n", row_counter++);
    }
    
    fclose(output_binary_file);
    fclose(output_data_file);
}

int is_decloration(char* line,FILE* output_data_file){
    
    sscanf(line, "%s", first_word); /*take the first word of the line*/

    if((strcmp(first_word,".define")==0)){/*if its a define command*/
        sscanf(line, "%*[^ ] %[^=]", first_word);/*Scan the first word before the = char, (the variable name)*/
        sscanf(line, "%*[^=]=%s", second_word); /*Scan the first word after the = char, (the variable value)*/
        /*Add validation check*/
        fprintf(output_data_file, "%s %s\n", first_word, second_word); /*Put the value to the DATA table*/
        return 1;
    }

    if(strcmp(first_word,".string")==0){
        return 1;
    }

    if(strcmp(first_word,".entry")==0){
        return 1;
    }

    if(strcmp(first_word,".extern")==0){
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

int add_label(char* line,FILE* output_data_file){

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

    fprintf(output_data_file, "%s %d\n", first_word, row_counter); /*Add the label name, and the row to data output file*/
    return 0;
}
