#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define MAX_WORD_SIZE 78

typedef struct command_list{ /*Linked node of commands*/
    char *data;
    struct command_list *next;
    } command_list;

typedef struct mcr_name{ /*linked node of MACROS*/
    char *name;
    struct command_list *first_command;
    struct mcr_name *next;
    }mcr_name ;

mcr_name* make_mcr(); /*Make new empty MACRO node function*/
command_list* make_cmd(); /*Make new empty command node function*/
  
/*------------------Compile Function------------------- */

int precompile(int line_length,FILE *input_file){
    
    FILE *output_file;
    char line[MAX_WORD_SIZE];
    char first_word[MAX_WORD_SIZE];
    mcr_name *head_mcr = NULL; /*An empty pointer to the first MACRO node*/
    mcr_name *curent_mcr = NULL; /*An empty pointer to track curent MACRO node*/
    command_list *curent_cmd = NULL; /*Same as curent_mcr but for command notes*/
    int hit = 0; /*MACRO name replace flag*/

    /*Make an output file*/
    output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        return 1;
    }


    while (fgets(line, sizeof(line), input_file)){ /*Read all the file,line by line */
        sscanf(line, "%s", first_word); /*Read the first word*/
        /*printf("%s",line);*/
        if (strcmp(first_word, "mcr") == 0) {
            hit=1; /*To make shure we dont wrigth the endmcrr */
            if (head_mcr == NULL){ /*If the head of the list is empty, make him*/
                head_mcr = make_mcr();
                curent_mcr = head_mcr;
                sscanf(line, "%*s %s", head_mcr->name); /*put the SECOND word, which is the name, in name feald*/
                /*printf("First hit %s\n",head_mcr->name);*/
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
            curent_mcr = head_mcr;
            /*printf("Got in while of %s\n", curent_mcr->name);*/
            while(curent_mcr!=NULL){
                printf("Name: %s\n",first_word);
                if(strcmp(first_word,curent_mcr->name)==0){ /*If this line is one of the MACRO names*/
                    curent_cmd=curent_mcr->first_command;/*Put the pointer to the first command of this MACRO node*/
                    /*printf("Hit on %s macro and %s\n",curent_mcr->name,first_word);*/
                    hit = 1;
                    fgets(line, sizeof(line), input_file); /*Delite the name Macro name from the file*/
                    while (curent_cmd!=NULL)/*Go thrue all of the nodes in this MACRO name*/
                    {  
                       fputs(curent_cmd->data, output_file);/*Add this line to the outpute file*/
                       curent_cmd = curent_cmd->next;/*GO to next command node*/
                    }
                    curent_mcr = NULL;
                }   
                else
                curent_mcr=curent_mcr->next;
            }
        }
        if(!hit){
            fputs(line, output_file);
        }
        hit=0;
          printf("Out of loop\n");
        /*fgets(line, sizeof(line), input_file);*/
    }
    /*printf("output.txt has been made");*/
    fclose(output_file);
    return 0;
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
