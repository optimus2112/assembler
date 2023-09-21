#ifndef labels_h
#define labels_h
#include "utl.h"
/*label.c file handels label realated operation and action, such as build label and check*/

/*Label table array. contains all the labels in Label data structure*/
LabelTable* l_table();

/*Returns the label from the label table.
If the label does NOT exitsts return NULL */
Label* get_label(char* );

/*Modifies 'dest' to cleared label name (without any spaces, and parenthesis)*/
void get_label_name(char* , char* );
/*Builds label definition*/
void build_label(char*);

#endif
