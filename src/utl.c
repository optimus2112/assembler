#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utl.h"
#include "line_check.h"
#include "labels.h"
#include "assembler.h"

/*Reallocates LabelTable's array*/
void realloc_lt(LabelTable* l, int size) {
    int i;
    Label** tmp = malloc(sizeof(Label*) * size);
    for (i = 0; i < l->capacity; ++i) {
        tmp[i] = l->array[i];
    }
    free(l->array);
    l->size *= 2;
    l->array = tmp;
}

/*Adds Label to LabelTable's array.
 * If the array is too small, its size is doubled*/
void add_to_label_table(LabelTable* t, Label* l) {
    if (t == NULL || l == NULL) return;
    if (get_label(l->name)) {
        /*The label allready defined*/
        return;
    }
    if (t->capacity == t->size) {
        /*Increase the capacity of the array*/
        realloc_lt(t, t->size*2);
    }
    t->array[t->capacity++] = l;

}

/*Allocates and initializes Label structure type on heap.*/
Label* create_label() {
    Label* l = malloc(sizeof(Label));
    l->base_address = RELOCATEBLE;
    l->name = NULL;
    l->content = NULL;
    l->attribute = UNDEFINED;
    l->offset = RELOCATEBLE;
    l->EE = NONE;
    return l;
}

/* Allocates on heap LabelTable structure type.
 * Must be free with 'free_label_table()' function after use.
 */
LabelTable* create_l_t() {
    LabelTable* l = malloc(sizeof(LabelTable));
    l->array = malloc(sizeof(Label*));
    l->size = 1;
    l->capacity = 0;
    return l;
}

/*Frees LabelTable s structure type*/
void free_label_table() {
    int i = 0;
    for (i = 0; i < l_table()->capacity; ++i) {
        free(l_table()->array[i]->content);
        free(l_table()->array[i]->name);
        free(l_table()->array[i]);
    }
    free(l_table()->array);
    free(l_table());
}

/*Frees Instruction structure type*/
void free_instruction(Instruction* ins) {
    if (ins == NULL) {
        fprintf(stderr, "Can't free null instruction\n");
        return;
    }
    if (ins->first_argument == NULL) {
        free(ins->second_argument);
        free(ins);
        return;
    }
    if (ins->second_argument == NULL) {
        free(ins->first_argument);
        free(ins);
        return;
    }
    free(ins->second_argument);
    free(ins->first_argument);
    free(ins);
}

/*Makes a copy of 'arg' and set it to first argument field in 'ins'*/
void set_ins_first_arg(Instruction* ins, char* arg) {
    char* tmp;
    if (arg == NULL) return;
    tmp = calloc(strlen(arg) + 1, 1);
    strcpy(tmp , arg);
    ins->first_argument = tmp;
}

/*Makes a copy of 'arg' and set it to second argument field in 'ins'*/
void set_ins_second_arg(Instruction* ins, char* arg) {
    char* tmp;
    if (arg == NULL) return;
    tmp = calloc(strlen(arg) + 1, 1);
    strcpy(tmp , arg);
    ins->second_argument = tmp;
}

/*Cleans label table:
 * frees all labels in array
 * sets 'capacity' field to 0
 * This function does NOT free interlay the label table
 * only clears it for the next run.*/
void clean_l_table() {
    int i;
    for (i = 0; i < l_table()->capacity; ++i) {
        free(l_table()->array[i]->content);
        free(l_table()->array[i]->name);
        free(l_table()->array[i]);
    }
    l_table()->capacity = 0;
}

/*Fills instruction fields. 'cp' MUST be copy of line*/
void fill_instruction(Instruction* com, char* cp) {
    int ins;
    ins = is_instruction(strtok(cp, " \n"));
    com->name = ins;
    com->first_argument = strtok(NULL, ",");
    com->second_argument = strtok(NULL,",");
    com->first_argument = strtok(com->first_argument, " \n");
    com->second_argument = strtok(com->second_argument, " \n");
    com->bin[0] = ABSOLUTE;
    com->bin[1] = ABSOLUTE;
}



