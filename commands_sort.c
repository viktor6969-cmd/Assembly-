#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define COMMAND_LIST_SIZE 16
#define MAX_LINE_SIZE 80
#define MAX_COMMAND_LINE_SIZE 14*3
#define FIRST_DIGITS "0000"

/*-------------STRUCTERS----------------*/
typedef struct commands{
    char* name;
    char* binary_code;
    char* operands;
}commands;

/*------------DECLARATIONS--------------*/
int make_command_list();

commands* command_list;


int command_data_write(char* line,FILE* output_file){
    
    return 0;
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
        if (list[i].binary_code == NULL || list[i].name == NULL) {
            printf("Error allocating memory for binary code or name");
            exit(EXIT_FAILURE);
        }
        sscanf(line, "%s %s %s", list[i].binary_code, list[i].name, list[i].operands);
    }
    printf("name: %s  operants: %s", list[0].name, list[0].operands);
    fclose(commands_file);
    command_list = list;
    return 0;
} 

void free_command_list() {
    int i;
    if (command_list == NULL) {
        return;
    }

    for (i = 0; i < COMMAND_LIST_SIZE; i++) {
        free(command_list[i].name);
        free(command_list[i].binary_code);
    }

    free(command_list);
    command_list = NULL;
}
