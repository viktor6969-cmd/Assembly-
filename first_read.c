#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

/*-------------STRUCTERS----------------*/

/*------------DECLARATIONS--------------*/
int add_string_node(char* name,char* line);
int add_data_node(char* name,char* line);
int label_validation_check(char* name);
int label_def(char* name,char* line);
int define_var(char* line);
int second_data_sort();
    

/*----------GLOBAL VARIABLES------------*/
binary* binary_output_curent = NULL;
binary* binary_output_head = NULL;
label* label_list_curent = NULL;
label* label_list_head = NULL;
char* file_name_glob;
int error_exist;
int temp_num;
int rows;
int IC;
int DC;

/*--------------Functions---------------*/
int first_read(FILE* input_file,char* file_name){

    label* temp;
    char line[MAX_LINE_SIZE];
    char first_word[MAX_LINE_SIZE];
    file_name_glob = file_name;
    error_exist = 0;
    rows = 1;
    IC = 100;
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
        temp = in_data_list(first_word,0); /*Cheack if it's a Label*/
        if(strcmp(first_word,".extern")==0){
            sscanf(line, "%*s %s", first_word);
            
            if(temp==NULL || (strcmp(temp->type,".extern")==0)){
                add_label(first_word,".extern","");
            }
            else{
                printf("ERROR in row %d:Double defenition for:%s\n",rows,first_word);
                error_exist=1;
            }
            rows++;
            continue;
        }
        
        /*-------------Check if entry defenition--------------------*/
        if(strcmp(first_word,".entry")==0){
            sscanf(line, "%*s %s", first_word);
            if(temp==NULL || (strcmp(temp->type,".entry")==0)){
                add_label(first_word,".entry","");
            }
            else{
                printf("ERROR in row %d:Double defenition for:%s\n",rows,first_word);
                error_exist++;
            }
            rows++;
            continue;
        }
        
        /*---------------------Check if label-----------------------*/
        if((first_word[strlen(first_word)-1]) == ':'){
            
            rows+=label_def(first_word,line);
            continue;
        }
        
        /*---------------------Check if comand ---------------------*/
        sscanf(line, "%*s %[^\n]", line);
        temp_num=command_sort(first_word,line,IC,rows);
        if(temp_num > 0){
            IC+=temp_num;
            rows++;
            continue;
        }
        /*If there any error (witch represented with negativa values) add it to error counter by making them positive*/
        error_exist+=(temp_num*-1); 
        rows++;
    }
    
    error_exist = second_data_sort();

    temp = label_list_head;
    /*while(temp!=NULL){
        printf("%s\t%s\t%s\n",temp->name,temp->type,temp->data);
        temp = temp->next;
    }
    printf("\n\n");
    
    while(binary_output_head!=NULL){
        printf("%s\t%c\t%d\n",binary_output_head->data,binary_output_head->type,binary_output_head->finished);
        binary_output_head = binary_output_head->next;
    }
    */
    if(error_exist > 0)
         return error_exist;

    printf_binary_files(binary_output_head,label_list_head,file_name);
    free_labels_list();
    return 0;
}

int define_var(char* line){

    char first_word[MAX_LINE_SIZE];
    char second_word[MAX_LINE_SIZE];

    sscanf(line, "%*s %[^= ]", first_word);/*Scan the first word before the = char, (the variable name)*/
    sscanf(line, "%*[^=]=%s",second_word); /*Read the second word after '=' ignoring white spaces*/
    
    /*-----------Validation check--------------*/
    label_validation_check(first_word);
    if(!is_number(second_word)){
        printf("Error in file: \'%s\' row %d:The value '%s' must be an integer number \n",rows++,second_word);
        error_exist++;
    }
    /*-------Add variable to labels list---------*/
    add_label(first_word,"mdefine",second_word);
    rows++; /*Encrease the rows number*/
    return error_exist;
}

int label_def(char* name,char* line){


    char first_word[MAX_LINE_SIZE];
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

    sprintf(first_word,"%d",IC); /*Use the first_word variable, to send the curent code row*/
    add_label(name,".data",first_word); /*Add code label to the list*/
    sscanf(line, "%s", name);
    sscanf(line, "%*s %[^\n]", line);
    temp_num=command_sort(name,line,IC,rows);
    if(temp_num>0)
        IC+=temp_num;
    else
        error_exist+=(temp_num*-1);
    return 0;
}

int add_string_node(char* name,char* line){
    /* ADD VALIDATION CHECK*/
    int i;
    char temp[MAX_BINARY_LINE_SIZE];
    sprintf(temp, "%d", DC+IC);
    add_label(name,".string",temp);
    for(i = 3;i<strlen(line)-3;i++){

        if((line[i] >= 'a' && line[i] <= 'z') || (line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= '0' && line[i] <= '9')){ 
            sprintf(temp,"0%d\t",IC+DC);/*Add zero to the rows number*/
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
    sprintf(temp,"0%d\t",IC+DC);/*Add zero to the rows number*/
    strcat(temp,char_to_binary('\0'));
    add_binary_line(temp,'d',0);
    DC++;
    return 0;
}

int add_data_node(char* name,char* line){
    int i;
    char word[MAX_LINE_SIZE-6]; /* The -6 is becouse we skipping the '.data'*/
    char temp[MAX_BINARY_LINE_SIZE];
    sprintf(word, "%d", DC+IC);
    add_label(name,".data",word);
    strcpy(word,"");
    for(i = 0;i<strlen(line);i++){
            if((line[i]==' ')||(line[i]=='+'))
                continue;

            if((line[i])==','){
                if((i+1) == strlen(line)){
                    printf("ERROR in file: \'%s\' row %d: line: %s \nMissing argument after \',\' in data declaration",file_name_glob,rows,line);
                    return 1;
                }
                if((line[i+1]==',')){
                    printf("ERROR in file: \'%s\' row %d: line: %s \nDubble \',\' char in data declaration",file_name_glob,rows,line);
                    return 1;
                }
                if(is_number(word)){ /*If it's a number add him*/
                    sprintf(temp,"0%d\t",IC+DC);/*Add zero to the rows number*/
                    strcat(temp,num_to_binary(atoi(word),14));/*Take the string 'word', cust him to int, and convert to binary, then add to temp*/
                    add_binary_line(temp,'d',0); /* Save in data list with row number*/
                    strcpy(word,"");
                    DC++;
                    continue;
                }
                if(in_data_list(word,0)!=NULL){ /*Check the labels list*/
                    sprintf(temp,"0%d\t",IC+DC);/*Add zero to the rows number*/
                    strcat(temp,num_to_binary(atoi(in_data_list(word,1)->data),14));
                    add_binary_line(temp,'d',0);
                    strcpy(word,"");
                    DC++;
                    continue;
                }
                else{
                    printf("ERROR in file:%s row %d: '%s' \nInvalid input\n",file_name_glob, rows,word);
                    return 1;
                }
                    
                strcpy(word,"");
            }
            else
                strncat(word,&line[i],1);
        }
    if(is_number(word)){ /*If it's a number add him*/
        sprintf(temp,"0%d\t",IC+DC);/*Add zero to the rows number*/
        strcat(temp,num_to_binary(atoi(word),14));
        add_binary_line(temp,'d',1);
         DC++;
    }
    if(in_data_list(word,0)!=NULL){ /*Check the labels list*/
        sprintf(temp,"0%d\t",IC+DC);/*Add zero to the rows number*/
        strcat(temp,num_to_binary(atoi(in_data_list(word,0)->data),14));
        add_binary_line(temp,'d',1);
        DC++;
    }
    return 0;
}

int second_data_sort(){

    binary *binary_temp = binary_output_head;
    label *label_temp = NULL;
    label *label_entry = NULL;
    char *word = malloc(MAX_BINARY_LINE_SIZE * sizeof(char));
    char *line = malloc(MAX_BINARY_LINE_SIZE * sizeof(char));

    while (binary_temp != NULL)
    {      
        if(binary_temp->type == 'u'){
            sscanf(binary_temp->data, "0%s %s",line,word);
            if((label_temp = in_data_list(word,2)) != NULL){
                if(strcmp(label_temp->type,".extern")==0){
                    sprintf(binary_temp->data,"0%s\t00000000000001",line);
                    add_label(label_temp->name,".extern_use",line);
                }
                 
                else
                    if((label_temp = in_data_list(word,2)) != NULL)
                        sprintf(binary_temp->data,"0%s\t%s10",line,string_to_binary(label_temp->data,12));                  
            }
            else {
                printf("ERROR: Undefined Label:\'%s\'\n",word);
                error_exist++;
            }
        }
        binary_temp = binary_temp->next;
    }
    
    label_temp = label_list_head;
    while (label_temp!=NULL)
    {
        if(strcmp(label_temp->type,".entry")==0){
            label_entry = in_data_list(label_temp->name,2);
            if(label_entry!=NULL){

                add_label(label_temp->name,".entry_use",label_entry->data);
            }
        }
        label_temp = label_temp->next;
    }
    

    return error_exist;
}


int add_label(char* name,char* type,char* data){

    label* new_label = (label*)calloc(1, sizeof(label));;
    if (new_label == NULL){ 
        print_error("Error allocating memory for the data tabel");
        error_exist++;
        return -1;
    }

    /*-------Allocate memory and fill it---------*/
    new_label->name = (char*)calloc(MAX_LABEL_NAME_SIZE, sizeof(char));
    strcat(new_label->name, name); /*Set the name*/
    new_label->type = (char*)calloc(MAX_LINE_SIZE, sizeof(char));
    strcat(new_label->type, type); /*Set the type to mdefine*/
    new_label->data = (char*)calloc(MAX_LINE_SIZE, sizeof(char));;
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
    binary* new_binary_line = (binary*)calloc(1,sizeof(binary));
    if (new_binary_line == NULL){ 
        print_error("Error allocating memory for the data tabel");
        error_exist ++;
        return 1;
    }
    new_binary_line->data = (char*)calloc(MAX_BINARY_LINE_SIZE, sizeof(char));
    strcat(new_binary_line->data,data);
    new_binary_line->finished = finished;
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

label* in_data_list(char* name,int data_flag){

    label* label_temp;
    if(label_list_head == NULL)
        return NULL; /*If the head is empty, the name can't be in the list*/

    label_temp = label_list_head;
    while(label_temp!=NULL){
        if(strcmp(label_temp->name,name)==0){
            switch (data_flag)
            {
            case 1:
            if(strcmp(label_temp->type,"mdefine")==0 || strcmp(label_temp->type,".extern")==0){
                return label_temp;
            }
            break;
            
            case 2:
            if((strcmp(label_temp->type,".entry")==0 || strcmp(label_temp->type,"entry_use")==0)){
                    label_temp = label_temp->next;
                    continue;
                }
            break;

            case 3:
            if((strcmp(label_temp->type,".entry")==0)){
                return label_temp;
            }
            default:
                break;
            }
            return label_temp;
        }
        label_temp = label_temp->next;
    }
    return NULL;
}

int label_validation_check(char* name){ 
    switch(defenition_name_valid_check(name,label_list_head)){

        case 1:
            printf("Error in line %d: Multiple definitions of label '%s'\n",rows++,name);
            error_exist++;
            break;

        case 2:
            printf("Error in line %d: The label '%s' can't have a name of assembly command\n",rows++,name);
            error_exist++;
            break;

        case 3:
            printf("Error in line %d: The label '%s' can't be a register name\n",rows++,name);
            error_exist++;
            break;

        case 4:
            printf("Error in line %d: The label '%s' can't be a number\n",rows++,name);
            error_exist++;
            break;
        
    }    
    return 0;
}

void free_labels_list(){

    while(label_list_head!=NULL){
        label_list_curent = label_list_head->next;
        free(label_list_head->name);
        free(label_list_head->data);
        free(label_list_head->type);
        free(label_list_head);
        label_list_head = label_list_curent;
    }
}