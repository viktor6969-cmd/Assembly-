#include <stdio.h>
#include "header.h"
// It works
int main(int argc, char *argv[]){
    
    if(InputValidCheck(argc,argv))
        return 1;
    return 0;
}