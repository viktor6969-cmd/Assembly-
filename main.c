#include <stdio.h>
#include "header.h"

#define MAX_LINE_LENGTH 80
int main(int argc, char *argv[]){

    int i;
    int mac;

    /*printf("\x1b[31m");*/
    if(InputValidCheck(argc,argv)) /*Check if the file exist and accessble*/
        return 1;

    make_command_list();
    for(i=1;i<argc;i++){
        mac = open_macros(argv[i]);/*Precompile function, returns 1 if file.am was made*/
        first_read(argv[i],mac);
    }
    free_command_list();
   /*compile_to_binary(macro_file);*/
    return 0;
}