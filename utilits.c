#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


char* string_to_binary(char* number, int bits){
    return number;
}

char* num_to_binary(int number, int bits){
    int i;
    char* binary = (char*)malloc(bits + 1); /* The +1 is for the null terminator*/

    for (i = bits - 1; i >= 0; i--) {
        binary[i] = (number & 1) ? '1' : '0';
        number >>= 1;
    }
    binary[bits] = '\0';/*Add the null terminator*/
    return binary; 
}
