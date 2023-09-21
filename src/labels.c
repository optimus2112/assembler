/*label.c file handels label realated operation and action, such as build label and check*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "utl.h"
#include "assembler.h"
#include "line_check.h"
#include "deractives.h"
#include "instructions.h"

/*Label table array. contains all the labels in Label data structure*/
LabelTable *l_table()
{
    static LabelTable *arr = NULL;
    if (arr == NULL)
        arr = create_l_t();
    return arr;
}

/*Check if tha label name is valid*/
Boolean label_name_check(char *name)
{
    int len = strlen(name);
    int i = 0;
    while (name[i] != '\0')
    {
        if (!isdigit(name[i]) && !isalpha(name[i]))
        {
            fprintf(stderr, "%s:%d: Invalid label name; only alphabet or digit charecters allowed\n", file_name(NULL), get_line_counter());
            set_errors_seen(SEEN);
            return FALSE;
        }
        ++i;
    }
    if (strlen(name) > MAX_LABEL_NAME_SIZE)
    {
        fprintf(stderr, "%s:%d: Invalid label name; Max name length is 31 charecters\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    if (is_instruction(name) != NONE || is_directive(name) != NONE)
    {
        fprintf(stderr, "%s:%d: Invalid label name; Label cannot be name as instruction or directive\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    if (get_reg(name) || !strcmp(name, "r0"))
    {
        fprintf(stderr, "%s:%d: Invalid label name; Label cannot be named as register\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    if (!strcmp(name, "macro") || !strcmp(name, "endm"))
    {
        fprintf(stderr, "%s:%d: Invalid label name; Label cannot be named as macro definition\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    while (i < len)
        if (isspace(name[i++]))
            return FALSE;
    return TRUE;
}

/*Returns the label from the label table.
If the label does NOT exitsts return NULL */
Label *get_label(char *name)
{
    char cp[BUFFER_SIZE];
    char *res;
    int i;
    if (name == NULL)
        return NULL;
    strcpy(cp, name);
    res = strtok(name, " \n");
    for (i = 0; i < l_table()->capacity; ++i)
    {
        if (!strcmp(res, l_table()->array[i]->name))
        {
            return l_table()->array[i];
        }
    }
    return NULL;
}

/*Builds label in deractive type*/
void build_labeled_dir(Label *l)
{
    int addres;
    addres = build_directive(l->content, l->attribute);
    if (addres == BUILD_FAILED)
        return;
    l->base_address |= addres - (addres % 16);
    l->offset |= (addres % 16);
}

/*Modifies 'dest' to cleared label name (without any spaces, and parenthesis)*/
void get_label_name(char *name, char *dest)
{
    char cp[BUFFER_SIZE];
    strcpy(cp, name);
    strcpy(dest, strtok(cp, " \n\t[:"));
}

/*Makes a copy of given name and set it as label name*/
void set_label_name(Label *l, char *name)
{
    char *tmp;
    int len;
    if (name == NULL)
        return;
    len = strlen(name);
    tmp = calloc(len + 1, 1);
    strcpy(tmp, name);
    l->name = tmp;
}

/*Makes a copy of given content and set it to label content*/
void set_label_content(Label *l, char *con)
{
    char *tmp;
    if (con == NULL)
        return;
    tmp = calloc(strlen(con) + 1, 1);
    strcpy(tmp, con);
    l->content = tmp;
}

/*Builds label when the assembler is at the first run*/
Label *build_label_first_pass(char *line)
{
    char cp[BUFFER_SIZE];
    Label *l = NULL;
    char *name = NULL;
    strcpy(cp, line);
    name = strtok(cp, ":");
    if (!label_name_check(name))
    {
        return NULL;
    }
    l = create_label();
    set_label_name(l, name);
    set_label_content(l, strtok(NULL, "\n"));
    l->attribute = line_builder(l->content, GET_TYPE);
    if (l->attribute == NONE)
    {
        fprintf(stderr, "%s:%d: Undefined command\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return NULL;
    }
    return l;
}

/*Builds label in instruction type*/
void build_labeled_ins(Label *l)
{
    int addres;
    char cp[BUFFER_SIZE];
    Instruction tmp;
    if (get_status() == SECOND_PASS)
    {
        addres = build_instruction(l->content);
        if (addres == BUILD_FAILED)
        {
            return;
        }
    }
    else
    {
        l->attribute = INSTRUCTION;
        l->base_address |= (get_ic() - get_ic() % 16);
        l->offset |= (get_ic() % 16);
        strcpy(cp, l->content);
        fill_instruction(&tmp, cp);
        inc_ic(get_ins_size(&tmp));
    }
}

/*Builds label definition*/
void build_label(char *line)
{
    Label *l = NULL;
    char label_name[BUFFER_SIZE];
    switch (get_status())
    {
    case (FIRST_PASS):
        l = build_label_first_pass(line);
        if (l == NULL)
            return;
        if (l->attribute == DATA || l->attribute == STRING || l->attribute == EXTERN)
        {
            build_labeled_dir(l);
        }
        if (l->attribute == INSTRUCTION)
        {
            build_labeled_ins(l);
        }
        add_to_label_table(l_table(), l);
        break;
    case (SECOND_PASS):
        get_label_name(line, label_name);
        l = get_label(label_name);
        if (l == NULL)
        {
            /*Invalid label definition, error has been printed before*/
            return;
        }
        if (l->attribute == INSTRUCTION)
        {
            build_labeled_ins(l);
        }
        else if (l->attribute == ENTRY)
        {
            build_labeled_dir(l);
        }
        break;
    default:
        fprintf(stderr, "%s:%d: Unknown status\n", file_name(NULL), get_line_counter());
    }
}
