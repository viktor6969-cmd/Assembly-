#include <stdio.h>
#include "header.h"

#define MAX_LINE_LENGTH 80
int main(int argc, char *argv[]){
    
    FILE *input_file;
    input_file = fopen(argv[1], "r");

    if(InputValidCheck(argc,argv))
        return 1;
    if(precompile(MAX_LINE_LENGTH,input_file))
        return 1;
    return 0;
}