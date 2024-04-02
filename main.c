#include <stdio.h>
#include "header.h"

#define MAX_LINE_LENGTH 80
int main(int argc, char *argv[]){

    int i;
    int mac;


    if(InputValidCheck(argc,argv)) /*Check if the file exist and accessble*/
        return 1;

    for(i=1;i<argc;i++){
        mac = open_macros(argv[i]);/*Precompile function, returns 1 if file.am was made*/

        /*first_values_read(macro_file);*/
        /*first_read(argv[i]);*/
    }
   /*compile_to_binary(macro_file);*/
    return 0;
}