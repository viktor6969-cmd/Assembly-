#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

/*-------------STRUCTERS----------------*/
typedef struct binary{
    char* data;
    char type;
    int finished;
    struct binary* next;
}binary;

/*------------DECLARATIONS--------------*/
int add_binary_line(char* data,char type,int finished);
int add_label(char* name,char* type,char* data);
int add_string_node(char* name,char* line);
int add_data_node(char* name,char* line);
int label_validation_check(char* name);
int label_def(char* name,char* line);
int define_var(char* line);
label* in_data_list(char* name);

/*----------GLOBAL VARIABLES------------*/
binary* binary_output_curent = NULL;
binary* binary_output_head = NULL;
label* label_list_curent = NULL;
label* label_list_head = NULL;
int error_exist;
int rows;
int IC;
int DC;

/*--------------Functions---------------*/
int first_read(FILE* input_file){
    
    char line[MAX_LINE_SIZE];
    char first_word[MAX_LINE_SIZE];
    error_exist = 0;
    rows = 1;
    IC = 0;
    DC = 0;


    while (fgets(line, sizeof(line), input_file)){ /*Go thrue the file, line by line*/
        
        sscanf(line, "%s", first_word); /*Read the first word*/  
        /*------------Skip all the comment lines--------------------*/
        if(line[0] == ';'){
            rows++;
            continue;
        }
        /*-----------Check if variable definition-------------------*/
        if(strcmp(first_word,".define")==0){
            define_var(line);
            rows++;
            continue;
        }
        /*------------Check if extern definition--------------------*/
        if(strcmp(first_word,".extern")==0){
            /*add_binary_line(first_word,'e',0);*/
            rows++;
            continue;
        }
        /*-------------Check if entry defenition--------------------*/
        if(strcmp(first_word,".entry")==0){
            /*add_binary_line(first_word,'e',0);*/
            rows++;
            continue;
        }
        /*---------------------Check if label----------------------*/
        if((first_word[strlen(first_word)-1]) == ':'){
            label_def(first_word,line);
            rows++;
            continue;
        }
        rows++;
    }
    
    while(label_list_head!=NULL){
        printf("%s\t%s\t%s\n",label_list_head->name,label_list_head->type,label_list_head->data);
        label_list_head = label_list_head->next;
    }
    printf("\n\n");
    while(binary_output_head!=NULL){
        printf("%s\t%c\t%d\n",binary_output_head->data,binary_output_head->type,binary_output_head->finished);
        binary_output_head = binary_output_head->next;
    }

    return error_exist;
}

int define_var(char* line){

    char first_word[MAX_LINE_SIZE];
    char second_word[MAX_LINE_SIZE];

    sscanf(line, "%*s %[^= ]", first_word);/*Scan the first word before the = char, (the variable name)*/
    sscanf(line, "%*[^=]=%s",second_word); /*Read the second word after '=' ignoring white spaces*/
    
    /*-----------Validation check--------------*/
    label_validation_check(first_word);
    if(!isNumber(second_word)){
        printf("Error in line %d:The value '%s' must be an integer number \n",rows++,second_word);
        error_exist = 1;
    }
    /*-------Add variable to labels list---------*/
    add_label(first_word,"mdefine",second_word);
    rows++; /*Encrease the rows number*/
    return error_exist;
}

int label_def(char* name,char* line){

    char first_word[MAX_LINE_SIZE];
    char comand_name[MAX_LINE_SIZE];
    name[strlen(name) - 1] = '\0'; /*Remove the ':' char*/
    label_validation_check(name); /*Validation check*/
    sscanf(line, "%*s %[^\n]", line);/*Remove the name of the label from the line*/
    sscanf(line, "%s", first_word); /*Read the first word*/  
    /*----------Check if data definition--------*/
    if(strcmp(first_word,".data")==0){
        sscanf(line, "%*s %[^\n]", line); /*Remove the '.data' from the line*/
        error_exist += add_data_node(name,line);/*Add every word to datat lisr*/
        return 0;
    }
    /*----------Check if string definition------*/
    if(strcmp(first_word,".string")==0){
        sscanf(line, "%*s %[^\n]", line); /*Remove the '.data' from the line*/
        error_exist += add_string_node(name,line);/*Add every word to datat lisr*/
        return 0;
    }

    sprintf(first_word,"%d",(IC+100)); /*Use the first_word variable, to send the curent code row*/ 
    add_label(name,"code",first_word); /*Add code label to the list*/
    sscanf(line, "%s", comand_name); /*Read the comand name*/  
    sprintf(first_word,"0%d\t0000",(IC++)+100);/*Add zero to the rows number*/
    strcat(first_word,command_code(comand_name));
    add_binary_line(first_word,'c',0);
    return 1;
}

int add_string_node(char* name,char* line){
    /* ADD VALIDATION CHECK*/
    int i;
    char temp[MAX_BINARY_LINE_SIZE];
    sprintf(temp, "%d", DC+IC+100);
    add_label(name,".string",temp);
    for(i = 3;i<strlen(line)-3;i++){

        if((line[i] >= 'a' && line[i] <= 'z') || (line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= '0' && line[i] <= '9')){ 
            sprintf(temp,"0%d\t",IC+DC+100);/*Add zero to the rows number*/
            strcat(temp,char_to_binary(line[i]));
            add_binary_line(temp,'d',0);
            DC++;
            continue;
        }
        else{
            printf("ERROR in row %d:Invalid char '%c' \n",rows,line[i]);
            return 1;
        }
    }
    sprintf(temp,"0%d\t",IC+DC+100);/*Add zero to the rows number*/
    strcat(temp,char_to_binary('\0'));
    add_binary_line(temp,'d',0);
    DC++;
    return 0;
}

int add_data_node(char* name,char* line){
    int i;
    char word[MAX_LINE_SIZE-6]; /* The -6 is becouse we skipping the '.data'*/
    char temp[MAX_BINARY_LINE_SIZE];
    sprintf(word, "%d", DC+IC+100);
    add_label(name,".data",word);
    strcpy(word,"");
    for(i = 0;i<strlen(line);i++){
            if((line[i]==' ')||(line[i]=='+'))
                continue;

            if((line[i])==','){
                if((i+1) == strlen(line)){
                    printf("ERROR in row %d: line: %s \nMissing argument after \',\' in data declaration",rows,line);
                    return 1;
                }
                if((line[i+1]==',')){
                    printf("ERROR in row %d: line: %s \nDubble \",\" char in data declaration",rows,line);
                    return 1;
                }
                if(isNumber(word)){ /*If it's a number add him*/
                    sprintf(temp,"0%d\t",IC+DC+100);/*Add zero to the rows number*/
                    strcat(temp,num_to_binary(atoi(word),14));/*Take the string 'word', cust him to int, and convert to binary, then add to temp*/
                    add_binary_line(temp,'d',0); /* Save in data list with row number*/
                    strcpy(word,"");
                    DC++;
                    continue;
                }
                if(in_data_list(word)!=NULL){ /*Check the labels list*/
                    sprintf(temp,"0%d\t",IC+DC+100);/*Add zero to the rows number*/
                    strcat(temp,num_to_binary(atoi(in_data_list(word)->data),14));
                    add_binary_line(temp,'d',0);
                    strcpy(word,"");
                    DC++;
                    continue;
                }
                else{
                    printf("ERROR in row %d: '%s' \nInvalid input\n",rows,word);
                    return 1;
                }
                    
                strcpy(word,"");
            }
            else
                strncat(word,&line[i],1);
        }
    if(isNumber(word)){ /*If it's a number add him*/
        sprintf(temp,"0%d\t",IC+DC+100);/*Add zero to the rows number*/
        strcat(temp,num_to_binary(atoi(word),14));
        add_binary_line(temp,'d',0);
         DC++;
    }
    if(in_data_list(word)!=NULL){ /*Check the labels list*/
        sprintf(temp,"0%d\t",IC+DC+100);/*Add zero to the rows number*/
        strcat(temp,num_to_binary(atoi(in_data_list(word)->data),14));
        add_binary_line(temp,'d',0);
        DC++;
    }
    return 0;
}




int add_label(char* name,char* type,char* data){

    label* new_label = malloc(sizeof(label));
    if (new_label == NULL){ 
        print_error("Error allocating memory for the data tabel");
        error_exist = -1;
        return -1;
    }

    /*-------Allocate memory and fill it---------*/
    new_label->name = malloc(MAX_LABEL_NAME_SIZE*sizeof(char));
    strcat(new_label->name, name); /*Set the name*/
    new_label->type = malloc(MAX_LINE_SIZE * sizeof(char));
    strcat(new_label->type, type); /*Set the type to mdefine*/
    new_label->data = malloc(MAX_LINE_SIZE * sizeof(char));
    strcat(new_label->data, data);/*Write the data*/
    new_label->next = NULL;

    /*-------Add to linked node list------------*/
    if(label_list_head == NULL){/*If haed empty make him to be head*/
        label_list_head = new_label;
        label_list_curent = new_label;
    }
    else{/*If there existing labels, add to the end of list*/
        label_list_curent->next = new_label;
        label_list_curent = new_label; 
    }
    return 0;
}

int add_binary_line(char* data,char type, int finished){
    binary* new_binary_line = malloc(sizeof(binary));
    if (new_binary_line == NULL){ 
        print_error("Error allocating memory for the data tabel");
        error_exist = -1;
        return -1;
    }
    new_binary_line->data = malloc(MAX_BINARY_LINE_SIZE*sizeof(char));
    strcat(new_binary_line->data,data);
    new_binary_line->finished = 0;
    new_binary_line->type = type;
    new_binary_line->next = NULL;

    if(binary_output_head == NULL){/*If haed empty make him to be head*/
        binary_output_head = new_binary_line;
        binary_output_curent = new_binary_line;
    }
    else{/*If there existing labels, add to the end of list*/
        binary_output_curent->next = new_binary_line;
        binary_output_curent = new_binary_line; 
    }
    /*printf("Table:%s\t%c\t%d\n",new_binary_line->data,new_binary_line->type,new_binary_line->finished);*/
    return 0;
}

label* in_data_list(char* name){

    label* label_temp;
    if(label_list_head == NULL)
        return NULL; /*If the head is empty, the name can't be in the list*/

    label_temp = label_list_head;
    while(label_temp!=NULL){
        if(strcmp(label_temp->name,name)==0)
            return label_temp;
        label_temp = label_temp->next;
    }
    return NULL;
}

int label_validation_check(char* name){
    switch(defenition_name_valid_check(name,label_list_head)){

        case 1:
            printf("Error in line %d: Multiple definitions of label '%s'\n",rows++,name);
            error_exist = 1;
            break;

        case 2:
            printf("Error in line %d: The label '%s' can't have a name of assembly command\n",rows++,name);
            error_exist = 1;
            break;

        case 3:
            printf("Error in line %d: The label '%s' can't be a register name\n",rows++,name);
            error_exist = 1;
            break;

        case 0:
            break;
    }    
    return 0;
}
