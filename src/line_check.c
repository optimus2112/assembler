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

/*Checks if the given string is well defined label.
If it is returns LABEL, NONE otherwise*/
LineTypes is_label(char* line) {
    char cp[BUFFER_SIZE];
    char* word;
    strcpy(cp, line);
    word = strtok(cp, ":");
    if (strcmp(word, line)) {
        return LABEL;
    }
    return NONE;
}

/*Returns parameter count of deractive*/
int get_param_count(char* arg, DirectivesTypes dir) { 
    int counter = 0;
    char cp[BUFFER_SIZE];
    if (arg == NULL) return 0;
    if(dir == STRING) {
        return strlen(arg) - 2;
    }else {
        counter = 1;
        strcpy(cp, arg);
        strtok(cp, " ,");
        while (strtok(NULL, " ,")) ++counter;
        return counter;
    }
    return -1;
}

/*Checks if the given command is instruction, if it is returns the type. otherwise returns NONE (-1)*/
InstructionsTypes is_instruction(char* string) {
    if (!strcmp(string, "add")) return ADD;
    if (!strcmp(string, "mov")) return MOV;
    if (!strcmp(string, "cmp")) return CMP;
    if (!strcmp(string, "sub")) return SUB;
    if (!strcmp(string, "lea")) return LEA;
    if (!strcmp(string, "clr")) return CLR;
    if (!strcmp(string, "inc")) return INC;
    if (!strcmp(string, "bne")) return BNE;
    if (!strcmp(string, "not")) return NOT;
    if (!strcmp(string, "jmp")) return JMP;
    if (!strcmp(string, "dec")) return DEC;
    if (!strcmp(string, "jsr")) return JSR;
    if (!strcmp(string, "red")) return RED;
    if (!strcmp(string, "prn")) return PRN;
    if (!strcmp(string, "rts")) return RTS;
    if (!strcmp(string, "stop")) return STOP;
    return NONE;
}

/*Checks if all the string is space*/
Boolean is_all_space(char* string) {
    int i = 0;
    if (string == NULL) return TRUE;
    while (string[i] != '\0' && i <= BUFFER_SIZE) {
        if (!isspace(string[i++])) return FALSE;
    }
    return TRUE;
} 

/*Returns line type;
 * Instruction
 * Comment
 * directive
 * label definition*/
int get_line_type(char* line) {
    char cp[BUFFER_SIZE];
    char* word;
    int flag;
    strcpy(cp, line);
    word = strtok(cp, " \n");
    flag = is_directive(word);
    if (flag != NONE) {
            return flag;
    }
    flag = is_instruction(word);/*Checking if the line is instruction*/
    if (flag != NONE) {
            return INSTRUCTION;
        } 
    flag = is_label(word);
    if (flag != NONE) {
            return LABEL;
        }
    return NONE;
}

/*Checks if the line is comment*/
Boolean is_comment(char* line) {
    int i = 0;
    while (isspace(line[i])) i++;
    if (line[i] == ';') return TRUE;
    return FALSE;
}

/*
modes:
GET_TYPE: return the type of the line
EXECUTE: builds the line*/
LineTypes line_builder(char* line, int mode) {
    char cp[BUFFER_SIZE];
    char* word = NULL;
    int line_type = 0;
    if (is_comment(line)) return COMMENT;
    strcpy(cp, line);
    word = strtok(cp, " \n");
    if (word == NULL) return EMPTY_LINE;
    if (mode == GET_TYPE) { 
        return get_line_type(line);
    }
    line_type = is_directive(word);
    if (line_type != NONE) {
        /*Line is directive*/
        build_directive(line, line_type);
        return OK;
    }
    line_type = is_instruction(word);
    if (line_type != NONE) {
        /*Line is intruction*/
        build_instruction(line);
        return OK;
    }
    line_type = is_label(word);
    if (line_type != NONE) {
        /*Line is label definition*/
        build_label(line);
    }
    return NONE;
}











