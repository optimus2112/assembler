/*
macro_builder.c handels all macro realated actions, such as 
creating .am file, and building the macro to that file.
*/
#ifndef m_builder
#define m_builder
typedef struct Macro_ {
    char* name;
    char* content;
} Macro;

typedef struct MacroArray_ 
{
    int size; /*total array size*/
    int capacity;/*actual count of macros in array*/
    Macro** array;
}MacroArray;


/*Creates .am file*/
FILE* create_mac_file();

/*Builds macro to 'out' file*/
void build_macro(FILE* input, FILE* out);
#endif




