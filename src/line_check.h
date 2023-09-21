#include "utl.h"
#ifndef line_check_h
#define line_check_h
/*Line check file handeles syntax checking of line.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "utl.h"
#include "deractives.h"
#include "instructions.h"
#include "labels.h"
#include "assembler.h"

/*Returns parameter count of deractive*/
int get_param_count(char* , DirectivesTypes );

/*Checks if the given command is instruction, if it is returns the type. otherwise returns NONE (-1)*/
InstructionsTypes is_instruction(char* );

/*Checks if all the string is space*/
Boolean is_all_space(char*);
/*
modes:
GET_TYPE: return the type of the line
EXECUTE: builds the line*/
LineTypes line_builder(char* , int );
#endif