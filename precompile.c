#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define MAX_WORD_SIZE 81

/*-------------------STRUCTURES-------------------------*/
typedef struct command_list{ /*Linked node of commands*/
    char *data;
    struct command_list *next;
    } command_list;

typedef struct mcr_name{ /*linked node of MACROS*/
    char *name;
    struct command_list *first_command;
    struct mcr_name *next;
    }mcr_name ;




/*--------------FUNCTIONS Declarations------------------*/
mcr_name* make_mcr(); /*Make new empty MACRO node function*/
command_list* make_cmd(); /*Make new empty command node function*/
void clear_maloc(mcr_name *head_mcr); /*Clears the memory alocated to all the dinamik variables*/


/*------------------Compile Function------------------- */
int open_macros(char *input_file_string){
    
    FILE *input_file;
    FILE *output_file;
    char line[MAX_WORD_SIZE];
    char first_word[MAX_WORD_SIZE];
    char file_name[MAX_WORD_SIZE];
    mcr_name *head_mcr = NULL; /*An empty pointer to the first MACRO node*/
    mcr_name *curent_mcr = NULL; /*An empty pointer to track curent MACRO node*/
    mcr_name *temp_mcr = NULL; /*An empty pointer to track curent MACRO node*/
    command_list *curent_cmd = NULL; /*Same as curent_mcr but for command notes*/
    int hit = 0; /*MACRO name replace flag*/

    /*Open the file*/
    
    sprintf(file_name,"%s.as",input_file_string);
    input_file = fopen(file_name,"r");

    /*Make an output file*/
    sprintf(file_name,"%s.am",input_file_string);
    output_file = fopen(file_name, "w");
    if (output_file == NULL) {
        printf("Error making the %s file",input_file_string);
        return 2;
    }


    while (fgets(line, sizeof(line), input_file)){ /*Read all the file,line by line */
        sscanf(line, "%s", first_word); /*Read the first word*/
        if (strcmp(first_word, "mcr") == 0) {
            hit=1; /*To make shure we dont wrigth the endmcrr */
            if (head_mcr == NULL){ /*If the head of the list is empty, make him*/
                head_mcr = make_mcr();
                curent_mcr = head_mcr;
                sscanf(line, "%*s %s", head_mcr->name); /*put the SECOND word, which is the name, in name feald*/
                fgets(line, sizeof(line), input_file); /*go to next line*/
            }
            else{ /*add macro node ot the end of the linked nodes*/
                mcr_name *new_mcr = make_mcr();
                sscanf(line, "%*s %s", new_mcr->name);
                curent_mcr->next = new_mcr;
                curent_mcr = new_mcr;
                /*printf("Next hit %s\n",new_mcr->name);*/
                fgets(line, sizeof(line), input_file); /*go to next line*/
            }
            sscanf(line, "%s", first_word);/*Get the first word in the line*/


            while (strcmp(first_word,"endmcr")!=0){    /*Go line by line, till you meet endmcr*/ 
                if(curent_mcr->first_command== NULL){ /*Check if this Node has any commands*/
                    curent_mcr->first_command = make_cmd();/*Make new empty command in curent MACRO node (this is the Head)*/
                    curent_cmd = curent_mcr->first_command; /*Make this curent command*/
                    strcpy(curent_cmd->data, line); /*Put he data in the first node*/
                }
                else{
                    curent_cmd->next = make_cmd();
                    curent_cmd = curent_cmd->next;
                    strcpy(curent_cmd->data, line);
                }
                fgets(line, sizeof(line), input_file);/*Go to the next line*/
                sscanf(line, "%s", first_word);/*Take the first line word*/
            }
        } 
        
        else if(head_mcr!=NULL){
            temp_mcr = head_mcr;
            /*printf("Got in while of %s\n", curent_mcr->name);*/
            while(temp_mcr!=NULL){
                /*printf("Name: %s\n",first_word);*/
                if(strcmp(first_word,temp_mcr->name)==0){ /*If this line is one of the MACRO names*/
                    curent_cmd=temp_mcr->first_command;/*Put the pointer to the first command of this MACRO node*/
                    /*printf("Hit on %s macro and %s\n",curent_mcr->name,first_word);*/
                    fgets(line, sizeof(line), input_file); /*Delite the name Macro name from the file*/
                    while (curent_cmd!=NULL)/*Go thrue all of the nodes in this MACRO name*/
                    {  
                       fputs(curent_cmd->data, output_file);/*Add this line to the outpute file*/
                       curent_cmd = curent_cmd->next;/*GO to next command node*/
                    }
                    temp_mcr = NULL;
                }   
                else
                temp_mcr=temp_mcr->next;
            }
        }
        if(!hit){
            fputs(line, output_file);
        }
        hit=0;
    }
    /*printf("output.txt has been made");*/
  
    fclose(output_file); /*Close the file, so he will be created*/
    printf("%s",file_name);
    if(head_mcr == NULL) /*If there was no macros, delite the file*/
        remove(file_name);
    clear_maloc(head_mcr); /*Clear the memory trash*/
    return 1;
}

mcr_name* make_mcr(){
    mcr_name *mcr_space = malloc(sizeof(mcr_name));
    if (mcr_space == NULL) {
        printf("Error allocating memory");
    }
    mcr_space->first_command = NULL;
    mcr_space->name = malloc(MAX_WORD_SIZE * sizeof(char));
    if (mcr_space->name == NULL) {
        printf("Error allocating memory");
    }
    mcr_space->next = NULL;
    return mcr_space;
}

command_list* make_cmd(){

    command_list *command = malloc(sizeof(command_list));
    if (command == NULL) {
        printf("Error allocating memory");
    }
    command->data = malloc(MAX_WORD_SIZE * sizeof(char));
    command->next = NULL;
    return command;
}

void clear_maloc(mcr_name *head_mcr) {
    mcr_name *temp_mcr;
    command_list *temp_cmd;

    while (head_mcr != NULL) {
        temp_mcr = head_mcr;
        head_mcr = head_mcr->next;

        while (temp_mcr->first_command != NULL) {
            temp_cmd = temp_mcr->first_command;
            temp_mcr->first_command = temp_mcr->first_command->next;
            free(temp_cmd->data);
            free(temp_cmd);
        }

        free(temp_mcr->name);
        free(temp_mcr);
    }
}