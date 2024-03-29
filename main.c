#include <stdio.h>
#include "header.h"

#define MAX_LINE_LENGTH 80
int main(int argc, char *argv[]){
    FILE *input_file;
    FILE *macro_file;
    input_file = fopen(argv[1], "r");

    if(InputValidCheck(argc,argv)) /*Check if the file exist and accessble*/
        return 1;

    if((macro_file = precompile(input_file,argv[1]))==NULL){ /*Precompile*/
        printf("Error to access the post compile file");
        return 1;
    }    

    /*first_values_read(macro_file);*/

    sort_commands(macro_file);

   /*compile_to_binary(macro_file);*/
    return 0;
}