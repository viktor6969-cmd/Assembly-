
int InputValidCheck(int argc, char *argv[]);
int open_macros(char *name);
int first_read(char* input_file_name, int mcr);
int command_data_write(char* line,int IC,FILE* output_file);
int isNumber(char *str);
int make_command_list();
void print_error(char* error_string);
void compile_to_binary(FILE *macro);
void free_command_list();


