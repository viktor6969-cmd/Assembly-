#include "stdio.h"
#include "header.h"

#define MAX_LINE_SIZE 80
#define MAX_COMMAND_LINE_SIZE 14*3

int InputValidCheck(int argc, char *argv[]){
    if (argc < 2){
        printf("The text file is missing");
        return 1;
    }

    while (argv[1] != 0){
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("Failed to open file %s\n", argv[1]);
            return 1;
        }
        fclose(file);
        argv++;
    }
    return 0;
}

int command_type(char* line){

    char first_word[MAX_LINE_SIZE];
    sscanf(line, "%s", first_word);
    return 0;
    
}


int isNumber(char *str) {
    if (str == NULL || *str == '\0') 
        return 1; 
    if (*str == '-') 
        str++; 
    while (*str != '\0') {
        if (*str < '0' || *str > '9') 
            return 1;        
        str++; 
    }
    return 0; 
}


