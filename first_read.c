#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

/*------------DECLARATIONS--------------*/
int add_string_node(char* name,char* line); /*Adds a string data to the commands list */
int add_data_node(char* name,char* line);
int label_validation_check(char* name);
int label_def(char* name,char* line);
int define_var(char* line);
int second_data_sort();
int link_data_list();
    

/*----------GLOBAL VARIABLES------------*/
binary* binary_output_curent = NULL;
binary* binary_output_head = NULL;
binary* binary_data_curent = NULL;
binary* binary_data_head = NULL;
label* label_list_curent = NULL;
label* label_list_head = NULL;
char* warning = "\033[0;35m WARNING\033[0m";
char* red_error = "\x1b[31m ERROR\033[0m";
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
    char second_word[MAX_LINE_SIZE];
    char third_word[MAX_LINE_SIZE];
    file_name_glob = file_name;
    error_exist = 0;
    rows = 0;
    IC = 0;
    DC = 0;


    while (fgets(line, sizeof(line), input_file)){ /*Go thrue the file, line by line*/
        
        rows ++;
        /*printf("Command:'%s' rows:%d\n",line,error_exist);*/
        /*------------Skip all the comment amd empty lines--------------------*/
        if(line[0] == ';' || strcmp(line, "\n") == 0)
            continue;
        
       
        /*----------------Sort commands by word number--------------------------------*/
        switch(sscanf(line, "%s %s %s",first_word,second_word,third_word)){/*Read the first word*/ 
        
        /*----------------------------Empty row---------------------------------------*/
        case 0:
            continue;


        /*-------------------------One word in row------------------------------------*/
        case 1:
            /*-----If it 'hlt' or 'rts' write the command------*/
            if(strcmp(first_word,"hlt") == 0 || strcmp(first_word,"rts")){

                temp_num=command_sort(first_word,line,IC+100,rows,file_name);
                if(temp_num > 0)
                    IC+=temp_num; 
                else
                    error_exist+=(temp_num*-1);
            }
            
            /*----If not print an error and go to next line----*/
            else{
                if(command_number(first_word)==-1)
                    printf("%s:%s:line %d Missing arguments for:\'%s\'\n",file_name_glob,red_error,rows,first_word);
                else
                    printf("%s:%s:line %d:Invalid line \'%s\'\n",file_name_glob,red_error,rows++,line);   
                
                error_exist++;
            }        
            continue;


        /*----------------------------Two words---------------------------------------*/
        case 2:    
       
            /*-----------Variable definition-------------------*/
            if(strcmp(first_word,".define")==0){
                define_var(line);
                continue;
            }

            /*------------Extern definition--------------------*/

            if(strcmp(first_word,".extern")==0){
                
                temp = in_data_list(second_word,0); 

                if(temp==NULL /*|| (strcmp(temp->type,".extern")==0)*/)
                    add_label(second_word,".extern","");
                
                else{
                    printf("%s:%s:line %d:Multiply defenition for:%s\n",file_name_glob,red_error,rows,second_word);
                    error_exist++;
                }
                continue;
            }
        
            /*-------------Entry defenition--------------------*/
            if(strcmp(first_word,".entry")==0){
               temp = in_data_list(second_word,3); 
                
                if(temp==NULL)
                    add_label(second_word,".entry","");
            
                else{
                    printf("%s:%s:line %d:Double defenition for:%s\n",file_name_glob,red_error,rows,second_word);
                    error_exist++;
                }
                continue;
            }
            
            /*--------------Label defenition-------------------*/
            if((first_word[strlen(first_word)-1]) == ':'){
                label_def(first_word,second_word);
                continue;
            }

            /*------------------Command------------------------*/
            temp_num=command_sort(first_word,second_word,IC+100,rows,file_name);

            /*------Check for errors and go to next line-------*/
            if(temp_num > 0)
                IC+=temp_num; 
            else   
                error_exist+=(temp_num*-1);/*If there any error (witch represented with negative values) add it to error counter by making them positive*/ 
            continue;
        

        /*-----------------------Thre and more words-----------------------------------*/
        case 3:

            /*-----------Variable definition-------------------*/
            if(strcmp(first_word,".define")==0){
                define_var(line);
                continue;
            }
            /*-------------Extern definition-------------------*/
            if(strcmp(second_word,".extern")==0){

                if(first_word[strlen(first_word)-1]==':'){
                    /*Write a warning*/
                    printf("%s:%s:line %d, incorrect definition of '%s'. A label cannot be defined as an '.extern' line\n",file_name,warning,rows,third_word);
                    /*Add him anyway, becouse we are nice */
                    temp = in_data_list(third_word,0); 

                    if(temp==NULL)
                        add_label(third_word,".extern","");
                    
                    else{
                        printf("%s:%s:line %d:Multiply defenition for:%s\n",file_name_glob,red_error,rows,third_word);
                        error_exist++;
                    }
                }
                else
                    printf("%s:%s:line %d:Invalid line:%s\n",file_name,red_error,rows,line);
                
                continue;
            }
            /*-------------Entry defenition--------------------*/
            if(strcmp(second_word,".entry")==0){

               if(first_word[strlen(first_word)-1]==':'){
                    /*Write a warning*/
                    printf("%s:%s:line %d, incorrect definition of '%s'. A label cannot be defined as an '.entery' line\n",file_name,warning,rows,third_word);
                    /*Add him anyway, becouse we are nice */
                    temp = in_data_list(third_word,0); 

                    if(temp==NULL){
                        add_label(third_word,".entry","");
                    }
                    else{
                        printf("%s:%s:line %d:Multiply defenition for:%s\n",file_name_glob,red_error,rows,third_word);
                        error_exist++;
                    }
                }
                else
                    printf("%s:%s:line %d:Invalid line:%s\n",file_name,red_error,rows,line);
                
                continue;
            }        
            /*--------------Label defenition-------------------*/
            if((first_word[strlen(first_word)-1]) == ':'){
                
                label_def(first_word,line);
               
                continue;
            }
            /*------------------Command------------------------*/
            sscanf(line, "%*s %[^\n]", line);/*Remove the command name from the command line*/
            
            if(sscanf(line,"%*s %*s %s",third_word) == 1){/*If tehre more then 2 arguments, print error and move on*/
                 printf("%s:%s:line %d:Unexpexted operand:'%s'\n",file_name_glob,red_error,rows,third_word);
                 error_exist ++;
            }
            temp_num=command_sort(first_word,line,IC+100,rows,file_name);

            /*------Check for errors and go to next line-------*/
            if(temp_num > 0)
                IC+=temp_num; 
            else 
                error_exist+=(temp_num*-1);/*If there any error (witch represented with negative values) add it to error counter by making them positive*/ 
            continue;

        /*--------------------------------Default--------------------------------------*/
        default:

            break;
        
        }
    }
    
    link_data_list();

    error_exist = second_data_sort();

    /* Connect data nodes to the end of command nodes */
        if (binary_data_head != NULL) {
            binary_output_curent->next = binary_data_head;
            binary_output_curent = binary_data_curent;
        }

    temp = label_list_head;

    printf("\n\n");
    while(temp!=NULL){
        printf("%s\t%s\t%s\n",temp->name,temp->type,temp->data);
        temp = temp->next;
    }
    printf("\n\n");
    
    while(binary_output_head!=NULL){
        printf("%s\t%c\t%d\n",binary_output_head->data,binary_output_head->type,binary_output_head->finished);
        binary_output_head = binary_output_head->next;
    }
    
    if(error_exist > 0) 
         return error_exist;

    printf_binary_files(binary_output_head,label_list_head,file_name,IC,DC);
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
        printf("%s:%s:line %d:The value '%s' must be an integer number \n",file_name_glob,red_error,rows++,second_word);
        error_exist++;
    }
    /*-------Add variable to labels list---------*/
    add_label(first_word,"mdefine",second_word);
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
        sscanf(line, "%*s %[^\n]", line); /*Remove the '.string' from the line*/
        error_exist += add_string_node(name,line);/*Add every word to datat lisr*/
        return 0;
    }

    sprintf(first_word,"%d",IC+100); /*Use the first_word variable, to send the curent code row*/
    add_label(name,"Label",first_word); /*Add code label to the list*/
    sscanf(line, "%s", name);
    sscanf(line, "%*s %[^\n]", line);
    temp_num=command_sort(name,line,IC+100,rows,file_name_glob);
    if(temp_num>=0)
        IC+=temp_num;
    else
        error_exist+=(temp_num*-1);
    return 0;
}

int add_string_node(char* name,char* line){

    int i;
    char temp[MAX_BINARY_LINE_SIZE];
    sprintf(temp, "%d", DC);
    add_label(name,".string",temp);
    if(line[0]=='\"' && line[strlen(line)-2]=='\"'){        
        for(i = 1;i<strlen(line)-2;i++){

            if((line[i] >= 'a' && line[i] <= 'z') || (line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= '0' && line[i] <= '9') || line[i] == ' '){ 
                sprintf(temp,"\t%s",char_to_binary(line[i]));
                add_binary_line(temp,'d',0);
                continue;
            }
            else{
                printf("%s:%s:line %d:Invalid char in string input'%c' \n",file_name_glob,red_error,rows,line[i]);
                error_exist ++;
            }
        }
    }
    else{
        printf("%s:%s:line %d:String input must start and end with with \" char \n",file_name_glob,red_error,rows);
        error_exist ++;
    }
    sprintf(temp,"\t%s",char_to_binary('\0'));
    add_binary_line(temp,'d',0);
    return 0;
}

int add_data_node(char* name,char* line){
    int i;
    char word[MAX_LINE_SIZE-6]; /* The -6 is becouse we skipping the '.data'*/
   
    sprintf(word, "%d", DC);
    add_label(name,".data",word);
    strcpy(word,"");
    for(i = 0;i<strlen(line);i++){
            if((line[i]==' ')||(line[i]=='+'))
                continue;

            if((line[i])==','){
                if((i+1) == strlen(line)){
                    printf("%s:%s:line %d:Missing argument after \',\' char in data declaration\n",file_name_glob,red_error,rows);
                    error_exist++;
                }
                if((line[i+1]==',')){
                    printf("%s:%s:line %d: %s\nDubble \',\' char in data declaration\n",file_name_glob,red_error,rows,line);
                   error_exist++;
                }
                temp_num=valid_data(word,file_name_glob,rows); 
                if(temp_num>=0)
                    error_exist+=(temp_num*-1);
                strcpy(word,"");
            }
            else
                strncat(word,&line[i],1);
        }

   
    temp_num=valid_data(word,file_name_glob,rows); 
    if(temp_num>=0)
        error_exist+=(temp_num*-1);
    return 0;
}

int second_data_sort(){

    binary *binary_temp = binary_output_head;
    label *label_temp = NULL;
    label *label_entry = NULL;
    char *word = (char*)calloc(MAX_BINARY_LINE_SIZE, sizeof(char));
    char *line = (char*)calloc(MAX_BINARY_LINE_SIZE, sizeof(char));

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
                if(command_number(word)>=0)
                    printf("%s:%s:Missing arguments for:\'%s\'\n",file_name_glob,red_error,word);
                else
                    printf("%s:%s:Undefined Label:\'%s\'\n",file_name_glob,red_error,word);
                error_exist++;
            }
        }
        
        if(binary_temp->type == 'i' && binary_temp->finished == 0){
            sscanf(binary_temp->data, "0%s %s",line,word);
            if((label_temp = in_data_list(word,2)) != NULL){
                if(strcmp(label_temp->type,".extern")==0){
                    sprintf(binary_temp->data,"0%s\t00000000000001",line);
                    add_label(label_temp->name,".extern_use",line);
                }
                 
                else if((label_temp = in_data_list(word,2)) != NULL)
                        sprintf(binary_temp->data,"0%s\t%s10",line,string_to_binary(label_temp->data,12));                  
            }
            else {
                
                printf("%s:%s:Undefined index:\'%s\'\n",file_name_glob,red_error,word);
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

int link_data_list(){

    label* temp = label_list_head;
    binary* bin_temp = binary_data_head;
    char* temp_str = (char*)calloc(MAX_LINE_SIZE, sizeof(char));
    while(temp!=NULL){
        if(strcmp(temp->type,".data")==0 || strcmp(temp->type,".string")==0)
            sprintf(temp->data,"%d",(atoi(temp->data) + IC + 100));
        temp= temp->next;
    }

    DC = 0;
    while(bin_temp!=NULL){
        sprintf(temp_str,"0%d%s",((DC++)+IC+100),bin_temp->data);
        strcpy(bin_temp->data, temp_str);
        bin_temp = bin_temp->next;
    }
    return 0;
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
    strcpy(new_label->name, name); /*Set the name*/
    new_label->type = (char*)calloc(MAX_LINE_SIZE, sizeof(char));
    strcpy(new_label->type, type); /*Set the type to mdefine*/
    new_label->data = (char*)calloc(MAX_LINE_SIZE, sizeof(char));;
    strcpy(new_label->data, data);/*Write the data*/
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

    if (type == 'd') {
        /* Data section */
        if (binary_data_head == NULL) {
            binary_data_head = new_binary_line;
            binary_data_curent = binary_data_head;
        }
        else {
            binary_data_curent->next = new_binary_line;
            binary_data_curent = new_binary_line;
        }
        DC++;
    }
    else {
        /* Code section */
        if (binary_output_head == NULL) {
            binary_output_head = new_binary_line;
            binary_output_curent = binary_output_head;
        }
        else {
            binary_output_curent->next = new_binary_line;
            binary_output_curent = new_binary_line;
        }
    }
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
                case 0:/*Just find the name in list*/
                    break;

                case 1:/*Find only defined or extern variables*/
                    if(strcmp(label_temp->type,"mdefine")==0 || strcmp(label_temp->type,".extern")==0){
                        return label_temp;
                    }
                    break;
            
                case 2:/*Skip entry and entry use labels*/
                    if((strcmp(label_temp->type,".entry")==0 || strcmp(label_temp->type,"entry_use")==0)){
                        label_temp = label_temp->next;
                        continue;
                    }
                    break;

                case 3:/*Find ONLY the entry and exten albels names*/
                    if((strcmp(label_temp->type,".entry")==0) || (strcmp(label_temp->type,".extern")==0)){
                        return label_temp;
                    }
                    label_temp = label_temp->next;
                    continue;

                case 4:/*Return only entry labels*/
                    if(strcmp(label_temp->type,".entry")==0){
                        label_temp = label_temp->next;
                        continue;
                    }
                    break;

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
            printf("%s:%s:line %d: Multiple definitions of label '%s'\n",file_name_glob,red_error,rows++,name);
            error_exist++;
            break;

        case 2:
            printf("%s:%s:line %d: The label '%s' can't have a name of assembly command\n",file_name_glob,red_error,rows++,name);
            error_exist++;
            break;

        case 3:
            printf("%s:%s:line%d: The label '%s' can't be a register name\n",file_name_glob,red_error,rows++,name);
            error_exist++;
            break;

        case 4:
            printf("%s:%s:line %d: The label '%s' can't be a number\n",file_name_glob,red_error,rows++,name);
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