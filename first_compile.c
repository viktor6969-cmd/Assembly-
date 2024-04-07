#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define MAX_LABEL_NAME_SIZE 31
#define MAX_LABEL_TYPE_SIZE 8
#define MAX_LINE_SIZE 80
#define MAX_COMMAND_LINE_SIZE 14*3

/*-------------STRUCTERS----------------*/
typedef struct labels{
    char* name;
    char* type;
    char* data;
    struct labels* next;
}labels;

/*------------DECLARATIONS--------------*/
int label_data_write(char* line,FILE* output_file);
int is_decloration(char* line, char* file_name);
int in_data_list(char* name);
int define_command(char* line);
labels* new_label();


/*----------GLOBAL VARIABLES------------*/
FILE* extern_file;
FILE* enter_file;
labels *labels_list_curent = NULL;
labels *labels_list_head = NULL;
labels *labels_temp;
char first_word[MAX_LINE_SIZE];
char second_word[MAX_LINE_SIZE];
char file_name_string[MAX_LINE_SIZE];
int IC;
int DC;


int first_read(char* input_file_name, int mcr){

    FILE *input_file;
    FILE *output_binary_file;
    char line[MAX_LINE_SIZE];
    IC = 100;
    DC = 0;

    if(mcr) /*Check if there is a macro file, if yes set the name to .am file*/
        sprintf(file_name_string,"%s.am",input_file_name);
    else /*If there is no macro file, set the name to the original file*/
        sprintf(file_name_string,"%s.as",input_file_name);

    input_file = fopen(file_name_string, "r"); /*Open the file*/

    sprintf(file_name_string,"%s.ob",input_file_name); /*Change the faile_name to file.ob*/
    output_binary_file = fopen(file_name_string, "w"); /*Open the output file to  write*/
    
    if (output_binary_file == NULL) /*Check if the file have been crieted*/
        print_error("Error creating object file");

/*--------------------Go line by line-------------------------------*/


    while (fgets(line, sizeof(line), input_file)){ /*Go thrue the file, line by line*/
        
        sscanf(line, "%s", first_word); /*Read the first word*/  
        if(line[0] == ';')/*Skip if it's a comment line*/
            continue;

/*---------------Check if definition--------------------------------*/

        if(strcmp(first_word,".define")==0){
            define_command(line);
            continue;
        }
/*---------------Check if decloration------------------------------*/
        switch(is_decloration(line,input_file_name)){/*Check if the command is of decloration type*/
            case -1: /*If it's a decloration, and the name is already exist in the data list, return error*/
                return 1;
            
            case 1: /*If the Name was added to the data list, keep to the next cycle*/
                continue;   
            
            default:
                break;
        }
/*---------------Check if Label----------------------------------*/
        sscanf(line, "%s", first_word);
        if((first_word[strlen(first_word)-1]) == ':'){ /*If the first word ends with ':' its a label*/
            
            first_word[strlen(first_word) - 1] = '\0'; /*Remove the ':' char*/
            if(in_data_list(first_word)){ /*Check if the Label exist in the Data table*/
                printf("ERROR: The Label \"%s\" defined more then once",first_word);
                return 1;
            }
            else{ /*If the label is new, check validation and add him*/           
                if(strlen(first_word)>MAX_LABEL_NAME_SIZE){ /*Check the size of the label*/
                    printf("The label name:%s is too long",line);
                    return 1;
                }
                labels_temp = new_label(); /*Make new label*/
                strcat(labels_temp->name,first_word); /*Add label name to datat tabel*/
                sscanf(line, "%*s %[^\n]", line); /*Remove the name of the label from the line*/
                sscanf(line, "%s", first_word);  /*read the first (next) word*/

                /*If the first word it's data definition*/
                if((strcmp(first_word,".data")==0)||((strcmp(first_word,".string")==0))){
                    strcat(labels_temp->type,"data");
                    sprintf(labels_temp->data, "%d",IC);
                }
                
                /*If not, send it to code_line fonction*/
                else{
                    strcat(labels_temp->type,"code"); /*Add type code*/
                    sprintf(labels_temp->data, "%d",IC); /*Add the line, and increase it*/
                    fprintf(output_binary_file, "%d\t0000%s\n", IC++,labels_temp->name);
                }
                
                if(labels_list_head == NULL){ /*If theis is the first Label in the file, make new head*/
                    labels_list_head = labels_temp;
                    labels_list_curent = labels_temp;
                }
                else{ /*If the head exist, just add him to the linked nodes*/
                    labels_list_curent->next = labels_temp;
                    labels_list_curent = labels_temp; 
                }
                if(label_data_write(line,output_binary_file)==-1){
                        printf("ERROR");
                        return -1;
                    }
                continue;
            }
        }
        else{
            command_data_write(line,IC,output_binary_file);
            fprintf(output_binary_file, "%d\t0000%s\n", IC++,first_word);
        }
    }
    labels_temp = labels_list_head;
    printf("\n");

   /*while(labels_temp!=NULL){
        printf("%s\t%s\t%s\n",labels_temp->name,labels_temp->type,labels_temp->data);
        labels_temp = labels_temp->next;
    }*/
    fclose(output_binary_file);

    return 0;
}

int label_data_write(char* line,FILE* output_file){
    int pos;
    sscanf(line, "%s", first_word);
    /*--------------ADD STRING CHECK-------------------*/
    if(strcmp(first_word,".string")==0){ /*If String deffination: Write down each char in aski*/
        for(pos = 11;pos<(strlen(line)-3);pos++)     
            fprintf(output_file, "%d\t%c\n", IC++,line[pos]);
        fprintf(output_file, "%d\t00000000000000\n", IC++); /*Add the \0 (end of file)*/
        return 0;
    }

    else if(strcmp(first_word,".data")==0){ /*If data defination: Check validation, replace names of varibels, and write it to the output file*/
        strcpy(first_word,"");

        for(pos = 6;pos<strlen(line);pos++){
    
            if((line[pos]==' ')||(line[pos]=='+'))
                continue;

            if((line[pos])==','){

                if(line[pos-1]==','){
                    printf("ERROR in line %s: \nDubble \",\" char in data declaration",line);
                    return -1;
                }
                else if((pos+1) == strlen(line)){
                    printf("ERROR in line \"%s\": \n Missing argument after \',\' in data declaration",line);
                    return -1;
                }
                if(in_data_list(first_word)) /* If the readen word is a name from the list, put the walue to the binary file */
                    fprintf(output_file, "%d\t%s\n",IC++,labels_temp->data);
                else
                    fprintf(output_file, "%d\t%s\n", IC++,first_word);
                strcpy(first_word,"");
            }
            else
                strncat(first_word,&line[pos],1);
        }
        
        fprintf(output_file, "%d\t%s\n", IC++,first_word);
        return 0;
    }

    return command_data_write(line,IC,output_file); /*If not data or string, send to the command sort function*/
}  

int define_command(char* line){

    sscanf(line, "%*s %[^= ]", first_word);/*Scan the first word before the = char, (the variable name)*/
    /*-----------Is the name exist already--------------*/
    if(defenition_name_valid_check(first_word))
        return -1;    
    if(!in_data_list(first_word)){
        labels_temp = new_label(); /*Make new label*/
        sscanf(line, "%*[^=]=%s", second_word); /*Scan the first word after the = char, (the variable value)*/
        strcat(labels_temp->name,first_word);
        strcat(labels_temp->type,"mdefine");
        strcat(labels_temp->data,second_word);
        if(labels_list_head == NULL){
            labels_list_head = labels_temp;
            labels_list_curent = labels_temp;
        }
        else{
            labels_list_curent->next = labels_temp;
            labels_list_curent = labels_temp; 
        }
        return 1;
    }

    else {
        printf("The variable name \"%s\" is used more than once\n",first_word);
        return -1;
    }
    /*Add validation check */
}

int is_decloration(char* line, char* input_file_name){

    sscanf(line, "%s", first_word); /*take the first word of the line*/
    

    if(strcmp(first_word,".entry")==0){
        sprintf(file_name_string, "%s.ent", input_file_name); /*Change the temp file name to file.ex*/
        extern_file = fopen(file_name_string,"a"); /* Open/create a file in add mode*/
        sscanf(line, "%*s %s", second_word); /*Take the second word after the .extern (the name)*/
        fprintf(extern_file, "%s\t%d\n",second_word,IC); /* FIX THE INPUTING*/
        fclose(extern_file); /*Close the file*/  
        return 1;
    }

    if(strcmp(first_word,".extern")==0){ 

        sprintf(file_name_string, "%s.ext", input_file_name); /*Change the temp file name to file.ex*/
        extern_file = fopen(file_name_string,"a"); /* Open/create a file in add mode*/
        sscanf(line, "%*s %s", second_word); /*Take the second word after the .extern (the name)*/
        fprintf(extern_file, "%s\t%d\n",second_word,IC); /* FIX THE INPUTING*/
        fclose(extern_file); /*Close the file*/  
        return 1;
    }

    return 0;
}

int in_data_list(char* name){

    if(labels_list_head == NULL)
        return 0; /*If the head is empty, the name can't be in the list*/

    labels_temp = labels_list_head;
    while(labels_temp!=NULL){
        if(strcmp(labels_temp->name,name)==0)
            return 1;
        labels_temp = labels_temp->next;
    }
    return 0;
}

labels* new_label(){
    labels* new_label = malloc(sizeof(labels));
    if (new_label == NULL) 
        print_error("Error allocating memory for the data tabel");   
    /*Initilaze space, and make shure there is no random values
    if(labels_list_head==NULL){
        labels_list_head = new_label;
        labels_list_curent = new_label;
    }*/
    new_label->name = malloc(MAX_LABEL_NAME_SIZE*sizeof(char));
    strcpy(new_label->name,"");
    new_label->type = malloc(MAX_LABEL_TYPE_SIZE*sizeof(char)); 
    strcpy(new_label->type,"");
    new_label->data = malloc(MAX_LINE_SIZE * sizeof(char));
    strcpy(new_label->data,"");
    new_label->next = NULL;
    
    return new_label;
}