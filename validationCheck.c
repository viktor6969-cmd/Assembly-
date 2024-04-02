#include "stdio.h"
#include <string.h>
#include "header.h"

#define MAX_LINE_SIZE 81
#define MAX_COMMAND_LINE_SIZE 14*3
FILE *file;
char file_name[MAX_COMMAND_LINE_SIZE];
int i;

int InputValidCheck(int argc, char *argv[]){
    if (argc < 2){
        print_error("The text file is missing");
        return 1;
    }
    
   for(i=1;i<argc;i++){

        sprintf(file_name,"%s.as",argv[i]);
        file = fopen(file_name, "r");
        if (file == NULL) {
            printf("\x1b[31mFailed to open file %s\x1b[0m\n", file_name);
            return 1;
        }
        fclose(file);
    }
    return 0;
}

int command_type(char* line){

    char first_word[MAX_LINE_SIZE];
    sscanf(line, "%s", first_word);
    return 0;
    
}

void print_error(char* error){
    printf("\x1b[31m%s \x1b[0m",error);
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


