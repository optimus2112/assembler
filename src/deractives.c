/*deractives.c file handels deractive realated actions, such as build and check deractive*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "utl.h"
#include "assembler.h"
#include "line_check.h"
#include "labels.h"


/*Checks if the string is a deractive*/
DirectivesTypes is_directive(char* string) {
    if (!strcmp(string, ".string")) return STRING;
    if (!strcmp(string, ".data")) return DATA;
    if (!strcmp(string, ".entry")) return ENTRY;
    if (!strcmp(string, ".extern")) return EXTERN;
    return NONE;
}

/*Checks if string's parameters are valid*/
Boolean string_check(char* arg) {
    int i;
    int len = strlen(arg);
    Boolean reached_ap = FALSE;
    if (is_all_space(arg)) {
        fprintf(stderr, "%s:%d: Empty string is not allowed\n", file_name(NULL), get_line_counter());
        return FALSE;
    }
    for (i = 0; i < len; ++i) {
        if (arg[i] == '"' && reached_ap) {
            reached_ap = FALSE;
            continue;
        }
        if (arg[i] == '"' && !reached_ap) {
            reached_ap = TRUE;
        } 
        if (!reached_ap && !isspace(arg[i])) {
            fprintf(stderr, "%s:%d: Invalid string definition\n", file_name(NULL), get_line_counter());
            return FALSE;
        }
    }
    if (reached_ap) {
        fprintf(stderr, "%s:%d: Invalid string definition\n", file_name(NULL), get_line_counter());
        return FALSE;
    }        
    return TRUE;
}


int get_comma_count(char* line) {
    int i = 0;
    int counter = 0;
    while (line[i] != '\0' && i <= BUFFER_SIZE) {
        if (line[i++] == ',') ++counter;
    }
    return counter;
}

/*Checks if the commas are currect at the deractives arguments*/
Boolean comma_check(char* line, int mode) {
    char cp[BUFFER_SIZE];
    char* word;
    char args[BUFFER_SIZE];
    int par = get_param_count(line, NONE);
    int com = get_comma_count(line);
    if (com >= par) return FALSE;
    strcpy(cp, line);
    word = strtok(cp, ",");
    while ((word = strtok(NULL, ","))) {
        if (is_all_space(word)){
            return FALSE;
        }
    }
    if (mode == INSTRUCTION) {
        strcpy(args, line);
        strtok(args, " ");
        word = strtok(NULL, "\0");
        if (get_param_count(word, NONE) >= 2) {
            strcpy(cp, line);
            if (!strcmp(strtok(cp, ",\0"),line)) return FALSE;
        }
    }
    return TRUE;
}

Boolean data_check(char* arg) {
    char* word = NULL;
    char cp[BUFFER_SIZE];
    if (!comma_check(arg, DERACTIVE)) {
        fprintf(stderr, "%s:%d: Invalid commas\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    strcpy(cp, arg);
    word = strtok(cp, ",");
    while((word != NULL)) {
        if (atoi(word) == 0 && *word != '0'){
            fprintf(stderr, "%s:%d: Data argument must be digits\n", file_name(NULL), get_line_counter());
            set_errors_seen(SEEN);
            return FALSE;
        }
        word = strtok(NULL, " ,");
    }
    return TRUE;

}

/*Checks if the arguments are valid for the currect deractive*/
Boolean directive_arg_check(char* arg, DirectivesTypes dir) {
    char cp[BUFFER_SIZE];
    char *word = NULL;
    if (arg == NULL) {
        fprintf(stderr, "%s:%d: No arguments for directive\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    if (dir == DATA) {
        return data_check(arg);
    }
    if (dir == STRING) {
        return string_check(arg); 
        }
    if ((dir == EXTERN || dir == ENTRY)) {
        if (is_all_space(arg)) {
            fprintf(stderr, "%s:%d: Missing operand\n", file_name(NULL), get_line_counter());
            set_errors_seen(SEEN);
            return FALSE;
        }
        if (get_param_count(arg, EXTERN) > 1) {
            fprintf(stderr, "%s:%d: Too many argument, only 1 required\n", file_name(NULL), get_line_counter());
            set_errors_seen(SEEN);
            return FALSE;
        }
        strcpy(cp, arg);
        word = strtok(cp, " ,");
        if (strcmp(word, arg)) {
            fprintf(stderr, "%s:%d: Invalid text after end of directive\n", file_name(NULL), get_line_counter());
            set_errors_seen(SEEN);
            return FALSE;
        }
    }
    return TRUE;
}

/*Returns an array of string, which every string is number from data's parameters*/
int* get_data_array(char* der_arg, int size) {
    int i;
    int* arr;
    char* word;
    char cp[BUFFER_SIZE];
    arr = calloc(size, sizeof(int));/*Creating array in the currect size*/
    strcpy(cp, der_arg);
    word = strtok(cp, " ,");
    i = 0;
    arr[i++] = (atoi(word) & ARE_CLEAR) | ABSOLUTE;
    while ((word = strtok(NULL, " ,"))) {
        if (i >= size) break;
        arr[i++] = (atoi(word) & ARE_CLEAR) | ABSOLUTE;
    }
    return arr;
}

/*Returns char array which is created from .string's argument*/
int* get_string_array(char* arg, int size) {
    int i;
    int len = strlen(arg);
    int* array;
    int j = 0;
    array = calloc(sizeof(int) * size, sizeof(int));
    for (i = 0; i < len; ++i) {
        if (arg[i] == '"') continue;
        array[j++] = arg[i] | ABSOLUTE;
    }
    array[j] = 0 | ABSOLUTE;
    return array;
}


int build_entry_dir(Label* l, char* args) {
    if (!(l = get_label(args))) {
        set_errors_seen(SEEN);
        fprintf(stderr, "%s:%d: The Label \"%s\" is undefined\n",file_name(NULL), get_line_counter(),args);
        return BUILD_FAILED;
    }
    if (l->EE == EXTERN) {
        fprintf(stderr, "%s:%d: The Label \"%s\" is allready defiend as external\n",file_name(NULL), get_line_counter(),args);
        return BUILD_FAILED;                
    }
    l->EE = ENTRY;
    return BUILD_SUCCEEDED;
}
int build_extern_dir(char* args) {
    Label* l = create_label();
    set_label_name(l, args);
    l->EE = EXTERN;
    l->offset =(l->offset & ARE_CLEAR) | EXTERNAL;/*Changing ARE bits*/
    l->base_address = (l->base_address & ARE_CLEAR) | EXTERNAL;
    add_to_label_table(l_table(), l);
    return BUILD_SUCCEEDED ; 
}

/*Builds deractive from line and type*/
int build_directive(char* line, DirectivesTypes dir) {
    int* array = NULL;
    int ptr = 0;
    Label* l = NULL;
    int size = 0;
    char cp[BUFFER_SIZE];
    char* args = NULL;
    strcpy(cp, line);
    args = strtok(cp, " ");
    args = strtok(NULL, "\n");/*Getting the directive's arguments*/
    if(!directive_arg_check(args, dir)) {
        set_errors_seen(SEEN);
        return BUILD_FAILED;
    }
    size = get_param_count(args, dir);/*Getting the size of memory allocation in data area*/
    switch(dir) {
        case(DATA):
            array = get_data_array(args, size);
            ptr = allocate_data(array, size);/*getting DC before memory allocation*/
            free(array);
            return ptr;  
        case(STRING):
            ++size;/*For Null charecter at the end of the string*/
            array = get_string_array(args, size);
            ptr = allocate_data(array, size);/*getting DC before memory allocation*/
            free(array);
            return ptr;
        case(ENTRY):
            return build_entry_dir(l, args);
        case(EXTERN):
            return build_extern_dir(args);
        default:
            fprintf(stderr, "%s:%d: Unknown directive\n", file_name(NULL), get_line_counter());
            return BUILD_FAILED;
    }
    
}

