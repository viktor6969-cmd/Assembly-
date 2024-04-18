#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

/*Convert string number to string binary*/
char* string_to_binary(char* str, int num_bits){

    /*Gets as input: a number, represented as string and number of bits to write
    return: a string in length of 'bits' witch is binary representetion of the number,
    all the missing bits are represented as '0' */

    char *binary_str = (char *)malloc((num_bits + 1) * sizeof(char));
    int num;
    int i;

    if (binary_str == NULL) {
        printf("Memory allocation error.\n");
        return NULL;
    }
    num= atoi(str);
    
    if (num < 0)
        num |= (1 << (num_bits - 1));

    
    for (i = num_bits - 1; i >= 0; i--) {
        binary_str[i] = (num & 1) ? '1' : '0';
        num >>= 1;
    }
    binary_str[num_bits] = '\0'; 
    return binary_str;
}

/*Convert integer to binary string*/
char* num_to_binary(int number, int bits){

    /*Gets an ineger and number of bits, and return a string representation of the number,
    in length of given bits, the extra bits replaced with '0'*/

    int i;
    char* binary = (char*)malloc(bits + 1); /* The +1 is for the null terminator*/

    for (i = bits - 1; i >= 0; i--) {
        binary[i] = (number & 1) ? '1' : '0';
        number >>= 1;
    }
    binary[bits] = '\0';/*Add the null terminator*/
    return binary; 
}

/*Convert char to binary string of aski representation*/
char* char_to_binary(char ch){

    /*Gets a char, and return a 14 chars srting, with the aski code of the char */
    
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
