
/*-----------Defenitions----------------*/
#define MAX_BINARY_LINE_SIZE 81 /*The maximum binary line: 4(maximum adress 4096)+4(tab)+14(binary code)*/
#define MAX_LABEL_NAME_SIZE 31 /*Maximum label size*/
#define MAX_LABEL_TYPE_SIZE 8 /*The longest label type is 'mdefine' which 7 chars*/
#define COMMAND_LIST_SIZE 16 /*Maximum size of the assembly command list */
#define MAX_LINE_SIZE 81 /*Longest size of line +1 for the '\0' char*/

/*----------------STRUCTERS-----------------*/
typedef struct label{
    /*Each label contain name,data and type:
    mdefine - variable defenition
    .data - data numnber
    .string - data string
    Label - command line label
    .extern - extern label
    .enter - enter label
    entry_use - usage of entry Label
    extern_use - usege of extern variable
    */
    char* name;/*The name of the label*/
    char* type;/*The type of the labe;*/
    char* data;/*Label data (if needed)*/
    struct label* next;
}label;

typedef struct binary{
    /*Each command contain binary data, type:
    c - comand
    u - undefined label
    i - index
    a - array representation
    r - register 
    e - extern
    h - hlt
    d - data
    */
    char* data;
    char type;
    int finished;
    struct binary* next;
}binary;


/*-----------Open_macros functions----------*/
/*Go line by line, and open all the macros, make a new file if found macros*/
int open_macros(char *name); 
/*------------------------------------------*/


/*------------Utilits functions-------------*/
/*Convert a num string into obinary code string of 'bits' lenth, replace all the extra space with 0*/
char* string_to_binary(char* number, int bits);
/*Converts binary nuber into binary string*/
char* num_to_binary(int number, int bits);
/*Converts a char, into binary aski code*/
char* char_to_binary(char ch);
/*------------------------------------------*/


/*--------Validation_check functions--------*/
/*Validation check for Labels*/
int defenition_name_valid_check(char* first_word, label* list);
/*Validation check for data input */
int valid_data(char* word,char* file_name,int rows);
/*Returns 1 if the given string is number and 0 if not*/
int is_number(char *str);
/*Returns 1 if the given string is register and 0 if not*/
int is_register(char* name);
/*------------------------------------------*/


/*-----------First_read functions-----------*/
/*Make new binary node, fill the given values, and link him to the end of the list*/
int add_binary_line(char* data,char type, int finished);
/*make new label node, fill the given values and link to the end of the list*/
int add_label(char* name,char* type,char* data);
/*Full read and comilation of the given file, returns an error counter, if there any errors, new files aren't made*/
int first_read(FILE* file,char* file_name);
/*Checks if given name exist in data list, filtererd by data_flag (more in the function discription)*/
label* in_data_list(char* name,int data_flag);
/*Free all the memory allocadet t olabel list*/
void free_labels_list();
/*------------------------------------------*/


/*------Commands_opeartions functions--------*/
/*make and allocate memmory tot new command list*/
int make_command_list();
/*Free the memory of existing command list*/
void free_command_list();
/*Returns a string with binary code of the given command,"" if command not found*/
char* command_code(char* name);
/*Returns an integer, represent array index of the given command, -1 if the command not in list at all*/
int command_number(char* name);
/*Sort the given commands, and write them in binary output list*/
int command_sort(char* name,char* line,int IC,int rows,char* file_name);
/*------------------------------------------*/


/*-----------Write_file functions------------*/
/*Translate the binary code into '%#!*', write him in .ob file, and create .ent and .ext files if needed*/
int printf_binary_files(binary *binary_list,label *label_list,char* file_name,int IC,int DC); 
/*------------------------------------------*/