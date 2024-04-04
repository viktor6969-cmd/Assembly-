#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define COMMAND_LIST_SIZE 16
#define MAX_LINE_SIZE 81
#define MAX_COMMAND_LINE_SIZE 14*3

/*-------------STRUCTERS----------------*/
typedef struct commands{
    char* name;
    char* binary_code;
    char* operands;
}commands;

/*------------DECLARATIONS--------------*/
int count_operands(char* line);

/*-------------VARIABLES----------------*/

commands* command_list;
int i;
/*------------FUNCTIONSS----------------*/
int command_data_write(char* line,int IC,FILE* output_file){ /*will return the number of lines for this command*/

    char first_word[MAX_LINE_SIZE];
    sscanf(line, "%s", first_word);
    for(i=0;i<COMMAND_LIST_SIZE;i++){
        if(strcmp(command_list[i].name,first_word)==0){
            fprintf(output_file, "%d\t0000%s", IC++,command_list[i].binary_code);
            
            count_operands(line);
        }
    }
    printf("The line: %s is invalid",line);
    return -1;
}

int make_command_list(){ /*Make space for new command list*/
    int i;
    FILE *commands_file;
    char line[MAX_LINE_SIZE];
    commands* list = malloc(COMMAND_LIST_SIZE * sizeof(commands)); 
    if (list == NULL) {
        printf("Error allocating memory for command list");
        return -1;
    }
    commands_file = fopen("commands.cm", "r");
    if (commands_file == NULL) {
        printf("Error opening the commands.txt file");
        return -1;
    }

    for (i = 0; i < COMMAND_LIST_SIZE; i++) {

        fgets(line, sizeof(line), commands_file);
        list[i].binary_code = malloc(4 * sizeof(char));
        list[i].name = malloc(MAX_LINE_SIZE * sizeof(char));
        list[i].operands = malloc(2 * sizeof(char));
        if (list[i].binary_code == NULL || list[i].name == NULL) {
            printf("Error allocating memory for binary code or name");
            exit(EXIT_FAILURE);
        }
        sscanf(line, "%s %s %s", list[i].binary_code, list[i].name, list[i].operands);
    }
    fclose(commands_file);
    command_list = list;
    return 0;
} 

int count_operands(char* line){

}

void free_command_list() {
    int i;
    if (command_list == NULL) {
        return;
    }

    for (i = 0; i < COMMAND_LIST_SIZE; i++) {
        free(command_list[i].name);
        free(command_list[i].binary_code);
        free(command_list[i].operands);
    }

    free(command_list);
    command_list = NULL;
}
