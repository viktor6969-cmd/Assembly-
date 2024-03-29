
int InputValidCheck(int argc, char *argv[]);
FILE* precompile(FILE *input_file,char *name);
void first_values_read(FILE *input_file);

void sort_commands(FILE* input_file);

void compile_to_binary(FILE *macro);
int isNumber(char *str);
void translater();
