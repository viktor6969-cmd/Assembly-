#include <stdio.h>
#include <stdlib.h>
#include "header.h"

int main(int argc, char *argv[]){

    int i;
    int errors;
    FILE* file;
    char file_name[MAX_LINE_SIZE];

    /*printf("\x1b[31m");*/
    /*-------Arguments check----------*/
    if(argc < 2){
        print_error("Arguments missing! \nusage : ./assembly \"file1\" \"file2\" ...");
        return 1;
    }

    make_command_list(); /*Make a list of commands*/
    /*--------Go file by file---------*/
    for(i=1;i<argc;i++){

        errors = 0;
        /*-------------Open file--------------------*/
        sprintf(file_name,"%s.as",argv[i]);
        file = fopen(file_name, "r");
        if (file == NULL) {
            printf("\x1b[31mFailed to open file %s\x1b[0m\n", file_name);
            return 1;
        }


        /*-------------Deploy Macros----------------*/
        if(open_macros(argv[i])){/*Precompile function, returns 1 if file.am was made*/
            sprintf(file_name,"%s.am",argv[i]);/*Change the name from file.as to file .am*/
            fclose(file);/*Close the original file.as*/
            file = fopen(file_name, "r");/*Open the new file.am, with all the macros*/
            if (file == NULL) {/*Check if the file exist*/
                printf("\x1b[31mFailed to open file %s\x1b[0m\n", file_name);
            return 1;
            }
        }

        /*------------First file read---------------*/
        errors += first_read(file);
        if(errors > 0)
            printf("\x1b[31mThere are %d errors in the sourcse file, outputile wasn't created\x1b[0m\n",errors);
        /*compile_to_binary(macro_file);*/

        fclose(file);
    }
    free_command_list();
    return 0;
}