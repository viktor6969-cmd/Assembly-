#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define MAX_LABEL_NAME_SIZE 31
#define MAX_LABEL_TYPE_SIZE 8
#define MAX_LINE_SIZE 80
#define MAX_COMMAND_LINE_SIZE 14*3

/*-------------STRUCTERS----------------*/
typedef struct labels{
    char* name;
    char* type;
    char* data;
    struct labels* next;
}labels;

/*------------DECLARATIONS--------------*/
void label_data_write(char* line,FILE* output_file);
int is_prog(char* line,FILE* output_data_file);
int is_decloration(char* line, char* file_name);
int add_label(char* line);
int in_data_list(char* name);
labels* new_label();


/*----------GLOBAL VARIABLES------------*/
FILE* extern_file;
FILE* enter_file;
labels *labels_list_curent = NULL;
labels *labels_list_head = NULL;
labels *labels_temp;


char first_word[MAX_LINE_SIZE];
char second_word[MAX_LINE_SIZE];
char file_name_string[MAX_LINE_SIZE];
int IC;
int DC;


int first_read(char* input_file_name, int mcr){

    FILE *input_file;
    FILE *output_binary_file;
    char line[MAX_LINE_SIZE];
    char first_word[MAX_LINE_SIZE];
    IC = 100;
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
/*---------------Check is decloration------------------------------*/
        switch(is_decloration(line,input_file_name)){/*Check if the command is of decloration type*/
            case -1: /*If it's a decloration, and the name is already exist in the data list, return error*/
                return 1;
            
            case 1: /*If the Name was added to the data list, keep to the next cycle*/
                continue;   
            
            default:
                break;
        }
        
          sscanf(line, "%s", first_word);

        if((first_word[strlen(first_word)-1]) == ':'){ /*If the first word ends with ':' its a label*/
            
            first_word[strlen(first_word) - 1] = '\0'; /*Remove the ':' char*/
            if(in_data_list(first_word)){ /*Check if the Label exist in the Data table*/
                printf("ERROR: The Label \"%s\" defined more then once",first_word);
                return 1;
            }
            else{
                /*If the label is new, check validation and add him*/
                if(strlen(first_word)>MAX_LABEL_NAME_SIZE){ /*Check the size of the label*/
                    printf("The label name:%s is too long",line);
                    return 1;
                }
                labels_temp = new_label(); /*Make new label*/
                strcat(labels_temp->name,first_word); /*Add label name to datat tabel*/
                sscanf(line, "%*s %[^\n]", line); /*Remove the name of the label*/
                sscanf(line, "%s", first_word);  /*read the first (next) word*/

                if((strcmp(first_word,".data")==0)||((strcmp(first_word,".string")==0))){/*If the first word it's data definition*/
                    strcat(labels_temp->type,"data");
                    sprintf(labels_temp->data, "%d",IC);
                    label_data_write(line,output_binary_file);
                }
                
                else{
                strcat(labels_temp->type,"code"); /*Add type code*/
                sprintf(labels_temp->data, "%d",IC); /*Add the line, and increase it*/
                fprintf(output_binary_file, "%d 0000%s\n", IC++,labels_temp->name);
                }
                
                if(labels_list_head == NULL){ /*If theis is the first Label in the file, make new head*/
                    labels_list_head = labels_temp;
                    labels_list_curent = labels_temp;
                }
                else{ /*If the head exist, just add him to the linked nodes*/
                    labels_list_curent->next = labels_temp;
                    labels_list_curent = labels_temp; 
                }
                continue;
            }
        }
        fprintf(output_binary_file, "%d 0000%s\n", IC++,first_word);
    }
    labels_temp = labels_list_head;
    /*while(labels_temp!=NULL){
        printf("%s\t%s\t%s\n",labels_temp->name,labels_temp->type,labels_temp->data);
        labels_temp = labels_temp->next;
    }*/
    fclose(output_binary_file);
    return 0;
}

void label_data_write(char* line,FILE* output_file){
    
    fprintf(output_file, "%d\t%s\n",IC++,first_word);
}

int is_decloration(char* line, char* input_file_name){

    sscanf(line, "%s", first_word); /*take the first word of the line*/
    if((strcmp(first_word,".define")==0)){/*if its a define command*/
        sscanf(line, "%*s %[^= ]", first_word);/*Scan the first word before the = char, (the variable name)*/
    /*-----------Is the name exist already--------------*/    
        if(!in_data_list(first_word)){
            labels_temp = new_label(); /*Make new label*/
            sscanf(line, "%*[^=]=%s", second_word); /*Scan the first word after the = char, (the variable value)*/
            strcat(labels_temp->name,first_word);
            strcat(labels_temp->type,"mdefine");
            strcat(labels_temp->data,second_word);
            if(labels_list_head == NULL){
                labels_list_head = labels_temp;
                labels_list_curent = labels_temp;
            }
            else{
                labels_list_curent->next = labels_temp;
                labels_list_curent = labels_temp; 
            }
            return 1;
        }

        else {
            printf("The variable name \"%s\" is used more than once\n",first_word);
            return -1;
        }
        /*Add validation check */
        
    }

    if(strcmp(first_word,".string")==0){
        return 1;
    }

    if(strcmp(first_word,".entry")==0){
        return 1;
    }

    if(strcmp(first_word,".extern")==0){ 

        sprintf(file_name_string, "%s.ex", input_file_name); /*Change the temp file name to file.ex*/
        extern_file = fopen(file_name_string,"a"); /* Open/create a file in add mode*/
        sscanf(line, "%*s %s", second_word); /*Take the second word after the .extern (the name)*/
        fprintf(extern_file, "%s  %d\n",second_word,(IC+100)); /* FIX THE INPUTING*/
        fclose(extern_file); /*Close the file*/  
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

int in_data_list(char* name){

    if(labels_list_head == NULL)
        return 0; /*If the head is empty, the name can be in the list*/

    labels_temp = labels_list_head;
    while(labels_temp!=NULL){
        if(strcmp(labels_temp->name,name)==0)
            return 1;
        labels_temp = labels_temp->next;
    }
    return 0;
}

labels* new_label(){
    labels* new_label = malloc(sizeof(labels));
    if (new_label == NULL) 
        print_error("Error allocating memory for the data tabel");   
    /*Initilaze space, and make shure there is no random values
    if(labels_list_head==NULL){
        labels_list_head = new_label;
        labels_list_curent = new_label;
    }*/
    new_label->name = malloc(MAX_LABEL_NAME_SIZE*sizeof(char));
    strcpy(new_label->name,"");
    new_label->type = malloc(MAX_LABEL_TYPE_SIZE*sizeof(char)); 
    strcpy(new_label->type,"");
    new_label->data = malloc(MAX_LINE_SIZE * sizeof(char));
    strcpy(new_label->data,"");
    new_label->next = NULL;
    
    return new_label;
}