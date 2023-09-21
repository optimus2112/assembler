/*
macro_builder.c handels all macro realated actions, such as 
creating .am file, and building the macro to that file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utl.h"
#include "macro_builder.h"
#include "assembler.h"
#include "line_check.h"
#include "deractives.h"


/*Creates macro*/
Macro* create_mac() {
    Macro* mac = malloc(sizeof(Macro));
    mac->content = (char*) calloc(1, 1);
    return mac;
}

/*Frees macro structure*/
void free_mac(Macro* mac) {
    free(mac->name);
    free(mac->content);
    free(mac);
}

/*Creates macro array*/
MacroArray* create_mac_arr() {
    MacroArray* mac = malloc(sizeof(MacroArray));
    mac->capacity = 0;
    mac->size = 1;
    mac->array = malloc(sizeof(Macro*));
    return mac;
}

/*Returns macro by name from macro array*/
Macro* get_mac(MacroArray* arr, char* name) {
    int i;
    for( i = 0; i < arr->capacity; ++i) {
        if (!strcmp(arr->array[i]->name, name)) return arr->array[i];
    }
    return NULL;
}


/*Adds macro to macro array*/
void add_to_mac_arr(MacroArray* mac_arr, Macro* mac) {
    if (mac_arr == NULL || mac == NULL) return;
    if (get_mac(mac_arr, mac->name) != NULL) {
        fprintf(stderr, "Macro %s is allready defined\n", mac->name);
        return;
    }
    if (mac_arr->capacity == mac_arr->size) {
        mac_arr->array = realloc(mac_arr->array, sizeof(Macro*) * (mac_arr->size * 2));
        mac_arr->size *= 2;
    }
    mac_arr->array[mac_arr->capacity++] = mac;
}

/*Check if the macro name is valid*/
Boolean macro_check(char* name) {
    int i;
    char cp[BUFFER_SIZE];
    int len = strlen(name);
    strcpy(cp, name);
    strtok(cp, " \n\t");
    if (get_reg(cp) || !strcmp(cp, "r0")) return FALSE;
    if (is_directive(cp) || is_instruction(name)) return FALSE;
    for (i = 0; i < len; ++i) {
        if (isspace(name[i])) return FALSE;
    }
    return TRUE;
}

/*frees macro array structure*/
void free_mac_arr(MacroArray* mac) {
    int i;
    for (i = 0; i < mac->capacity; ++i) {
        free_mac(mac->array[i]);
    }
    free(mac->array);
    free(mac);
}

/*Adds content to existing macro content*/
void add_content(Macro* mac, char* con) {
    int len = strlen(mac->content) + strlen(con);
    mac->content = (char*) realloc(mac->content, len + 1);
    strcat(mac->content, con);
}

/*Creating macro output file, and updating the global output file name*/
FILE* create_mac_file() {
    char* am;
    FILE* file;
    int len;
    if (file_name(NULL) == NULL) return NULL;
    len = strlen(file_name(NULL));
    am = calloc(len + 4, 1);
    strcpy(am, file_name(NULL));
    strcat(am, ".am");
    file = fopen(am, "w");/*Overwriting all the prevues content in the file*/
    fclose(file);
    file = fopen(am, "a+");
    free(am);
    return file;
}

/*Sets macro name to given argument*/
void set_mac_name(Macro* mac, char* name) {
    char* cp;
    if (mac == NULL || name == NULL) return;
    cp = calloc(strlen(name) + 1, 1);
    strcpy(cp, name);
    cp = strtok(cp, " ");
    cp = strtok(cp, "\n");
    mac->name = cp;
}

/*Copying macro's content*/
Macro* copy_mac(Macro* mac) {
    Macro* tmp = create_mac();
    set_mac_name(tmp, mac->name);
    add_content(tmp, mac->content);
    return tmp;
}

/*Initializing macro by name*/
Macro* add_new_mac(char* buffer) {
    char* cp;
    Macro* mac = create_mac();
    cp = (char*) calloc(strlen(buffer) + 1, 1);
    strcpy(cp, buffer);
    strtok(cp, " ");
    set_mac_name(mac, strtok(NULL, " "));
    free(cp);
    return mac;
}

void build_macro(FILE* input, FILE* out) {
    char buffer[BUFFER_SIZE];
    char cp[BUFFER_SIZE];
    char* first_field = NULL;
    Macro* mac = NULL;
    MacroArray* mac_arr = create_mac_arr();
    Boolean in_def = FALSE;
    while (fgets(buffer, BUFFER_SIZE, input) != NULL) {
        strcpy(cp, buffer);
        /*Getting the first field of the line*/
        first_field = strtok(cp, " \n");
        if (first_field == NULL) goto skip;
        if (in_def) { /*in macro definition*/
            if (!strcmp(first_field, "endm")) {
                /*Reached end of macro definition*/
                in_def = FALSE;
                add_to_mac_arr(mac_arr, copy_mac(mac));
                free_mac(mac);
                continue;
            }
            add_content(mac, buffer);/*Adding the line to macro's content*/
            continue;
        }
        else if (!strcmp(first_field, "macro")) {
            /*Reached macro definition*/
            in_def = TRUE;
            mac = add_new_mac(buffer);
        }
        else if ((mac = get_mac(mac_arr, first_field))) {
            /*Reached macro name*/
            fprintf(out, "%s", mac->content);
        } else {
            goto skip;
        }
        skip: fprintf(out, "%s", buffer);
    }
    free_mac_arr(mac_arr);
    fseek(out, 0, SEEK_SET);/*Setting the file pointer to the begining*/
}
