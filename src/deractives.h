/*deractives.c file handels deractive realated actions, such as build and check deractive*/

#include "utl.h"
#ifndef deractives_h
#define deractives_h
/*Builds directive*/
int build_directive(char*, DirectivesTypes);

/*Checks if line is directive*/
DirectivesTypes is_directive(char*);

/*Checks the argument of directive*/
Boolean directive_arg_check(char* , DirectivesTypes );
#endif