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

char* char_to_binary(char ch){
    int i;
    int ascii = (int)ch; 
    char *binary = (char *)malloc(15); 

    if (binary == NULL) {
        printf("Memory allocation failuer.\n");
        return NULL;
    }

    binary[14] = '\0';

    for (i = 0; i < 14; i++) {
        binary[i] = (ascii & (1 << (13 - i))) ? '1' : '0'; 
    }

    return binary;
}

void print_error(char* error){
    printf("\x1b[31m%s \x1b[0m",error);
}
