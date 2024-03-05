#include "stdio.h"
#include "header.h"

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

