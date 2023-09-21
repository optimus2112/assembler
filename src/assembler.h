/*assembler.c file contains all assembler realated function:
 * memory allocation
 * register checking
 * first pass and second pass
 * data and instruction img*/

#ifndef assembler_h
#define assembler_h
/*assembler.c file handels all the main assembler operation the actions, such as:
memory allocation, creating output files*/

#include <stdio.h>
#define GET -1
/*Returns register number of the given register*/
int get_reg(char* );

/*Returns if .ext is allready created*/
Boolean is_ext_created(int );

/*Returns if .ent is allready created*/
Boolean is_ent_created(int );

/*allocates memory in data image*/
int allocate_data(int* , int );

/*Data image array*/
int* data_img();

/*instruction image array*/
int* instructions_img();

/*allcates memory in instruction image*/
int allocate_instruction(int* , int );

/*Returns data counter*/
int get_dc();

/*Incremenate data counter in number of words*/
void inc_dc(int );

/*Returns instruction counter*/
int get_ic();

/*Incremenate instruction counter in number of word*/
void inc_ic(int);

/*Changes the assembler status*/
void set_status(int);

/*Returns assembler status*/
int get_status();

/*Returns line counter*/
int get_line_counter();

/*Handles the file name;
if given arguments is not NULL, applays this name as file name, otherwise returns the current fil name*/
char* file_name(char*);

/*Adds label to entry file*/
void add_to_entry(Label* );

/*Adds label to external file*/
void add_to_extern(char* , int );

/*assembler status which indicates if the assembler is allowed to preduce object files*/
int errors_seen(int*  );

int get_errors_seen();/*Change name*/


void set_errors_seen(int  );

/*main assembler function
 * Assembler function pass on source code twice:
 * First time to build directives and get the total size of instruction image
 * Second time to build instructions
 * If Errors ocurred in source code, does not build object files*/
void assembler(FILE*);
#endif
 

