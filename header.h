
/*-----------Defenitions----------------*/
#define MAX_BINARY_LINE_SIZE 30 /*The maximum binary line: 4(maximum adress 4096)+4(tab)+14(binary code)*/
#define MAX_LABEL_NAME_SIZE 31 /*Maximum label size*/
#define MAX_LABEL_TYPE_SIZE 8 /*The longest label type is 'mdefine' which 7 chars*/
#define COMMAND_LIST_SIZE 16 /*Maximum size of the assembly command list */
#define MAX_LINE_SIZE 81 /*Longest size of line +1 for the '\0' char*/

/*----------------STRUCTERS-----------------*/
typedef struct label{
    char* name;
    char* type;
    char* data;
    struct label* next;
}label;

/*-----------Open_macros functions---------*/
int open_macros(char *name);

/*------------Utilits functions-------------*/
char* string_to_binary(char* number, int bits);
char* num_to_binary(int number, int bits);
char* char_to_binary(char ch);

/*--------Validation_check functions--------*/
int defenition_name_valid_check(char* first_word, label* list);
void print_error(char* error_string);
int is_number(char *str);

/*-----------First_read functions-----------*/
int add_binary_line(char* data,char type, int finished);
int add_label(char* name,char* type,char* data);
int first_read(FILE* file);

/*------Commands_opeartions functions--------*/
int make_command_list();
void free_command_list();
char* command_code(char* name);
int command_sort(char* name,char* line,int IC);





int command_data_write(char* line,int IC,FILE* output_file);
label* in_data_list(char* name);
void compile_to_binary(FILE *macro);



