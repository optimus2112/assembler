#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "utl.h"
#include "assembler.h"
#include "line_check.h"
#include "labels.h"
#include "deractives.h"

int get_word_bin(Instruction *ins);

int get_reg_from_label(char *label)
{
    char *tmp = NULL;
    char cp[BUFFER_SIZE];
    int reg = -1;
    if (label == NULL)
        return 0;
    strcpy(cp, label);
    tmp = strtok(cp, "[");
    tmp = strtok(NULL, "[");
    tmp = strtok(tmp, "]");
    if (tmp == NULL)
        return 0;
    reg = get_reg(tmp);
    if (reg == 0 && strcmp(tmp, "r0"))
        return NONE;
    return reg;
}

/*Returns the type of an argument of an instruction;
REGISTER, NUMBER, LABEL, NONE*/
int get_arg_type(char *arg)
{
    char cp[BUFFER_SIZE];
    char cp2[BUFFER_SIZE];
    char *tmp;
    char *name = NULL;
    int id;
    if (arg == NULL)
        return NONE;
    /*Checking if the first arg is register*/
    strcpy(cp, arg);
    name = strtok(cp, "\n");
    id = get_reg(name);
    if (id != 0 || !strcmp(name, "r0"))
    {
        return REGISTER;
    }
    /*Checking if the first arg is number*/
    strcpy(cp2, arg);
    tmp = strtok(cp2, "#");
    if (strcmp(tmp, arg))
    {
        return NUMBER;
    }
    return LABEL;
}

/*Returns the funct value of an instruction*/
int get_funct(InstructionsTypes ins)
{
    switch (ins)
    {
    case (ADD):
    case (CLR):
    case (JMP):
        return ADD_CLR_JMP_FUNCT << 12;
    case (SUB):
    case (NOT):
    case (BNE):
        return SUB_NOT_BNE_FUNCT << 12;
    case (INC):
    case (JSR):
        return INC_JSR_FUNCT << 12;
    case (DEC):
        return DEC_FUNCT << 12;
    default:
        return 0;
    }
    return 0;
}

/*Returns the opcode of an instruction*/
int get_opcode(InstructionsTypes ins)
{
    switch (ins)
    {
    case (MOV):
        return 1 << MOV_OPCODE;
    case (CMP):
        return 1 << CMP_OPCODE;
    case (ADD):
    case (SUB):
        return 1 << ADD_SUB_OPCODE;
    case (LEA):
        return 1 << LEA_OPCODE;
    case (CLR):
    case (NOT):
    case (DEC):
    case (INC):
        return 1 << BIT_OPERATIONS;
    case (JMP):
    case (BNE):
    case (JSR):
        return 1 << JMP_BNE_JSR_OPCODE;
    case (RED):
        return 1 << RED_OPCODE;
    case (PRN):
        return 1 << PRN_OPCODE;
    case (RTS):
        return 1 << RTS_OPCODE;
    case (STOP):
        return 1 << STOP_OPCODE;
    default:
        return 0;
    }
}

/*Checks if the instruction is valid (which has two arguments)*/
Boolean two_args_check(Instruction *ins)
{
    if (ins->second_argument == NULL || ins->first_argument == NULL)
    {
        fprintf(stderr, "%s:%d: Missing 1 argument out of 2 for instruction\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    if (get_arg_type(ins->second_argument) == NUMBER && ins->name != CMP)
    {
        fprintf(stderr, "%s:%d: Invalid argument for instruction\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    if (get_word_bin(ins) == NONE)
    {
        fprintf(stderr, "%s:%d: Invalid register\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    return TRUE;
}

/*Checks if the instruction is valid (which has one arguments)*/
Boolean single_arg_check(Instruction *ins)
{
    if (ins->first_argument == NULL)
    {
        fprintf(stderr, "%s:%d: Missing 1 argument out of 1 for instruction\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    if (ins->second_argument != NULL && !is_all_space(ins->second_argument))
    {
        fprintf(stderr, "%s:%d: Too many argument, only 1 required\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    if ((get_arg_type(ins->first_argument) == NUMBER) && ins->name != PRN)
    {
        fprintf(stderr, "%s:%d: Invalid argument for instruction\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    if ((get_arg_type(ins->first_argument) == REGISTER) && ins->name == JMP)
    {
        fprintf(stderr, "%s:%d: Invalid argument for instruction\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    if (get_word_bin(ins) == NONE)
    {
        fprintf(stderr, "%s:%d: Invalid register\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    return TRUE;
}

/*Checks if the instruction is valid (which has no arguments)*/
Boolean no_args_check(Instruction *ins)
{
    if (ins->first_argument != NULL || ins->second_argument != NULL)
    {
        fprintf(stderr, "%s:%d: too many arguments, 0 required\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    return TRUE;
}

/*Checks the arguments of instruction*/
Boolean check_ins_args(Instruction *ins)
{
    switch (ins->name)
    {
    case (MOV):
    case (CMP):
    case (ADD):
    case (SUB):
    case (LEA):
        return two_args_check(ins);
    case (CLR):
    case (NOT):
    case (INC):
    case (DEC):
    case (JMP):
    case (BNE):
    case (JSR):
    case (RED):
    case (PRN):
        return single_arg_check(ins);
    case (RTS):
    case (STOP):
        return no_args_check(ins);
    default:
        fprintf(stderr, "%s:%d: Unsupported instruction\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
}

/*Returns the total size of an instruction (in word wise)*/
int get_ins_size(Instruction *instr)
{
    int size = 2;
    if (get_arg_type(instr->first_argument) == LABEL || get_arg_type(instr->second_argument) == LABEL)
        size = 4;
    if (get_arg_type(instr->first_argument) == NUMBER || get_arg_type(instr->second_argument) == NUMBER)
        size++;
    if (get_arg_type(instr->first_argument) == LABEL && get_arg_type(instr->second_argument) == LABEL)
        size = 6;
    if (instr->name == STOP || instr->name == RTS)
        size = 1;
    return size;
}

/*Returns the addresing mode of an operand*/
int get_addresing_mode(char *arg)
{
    char *cp;
    char *tmp;
    int id = get_arg_type(arg);
    switch (id)
    {
    case (NUMBER):
        return IMMEDIATE;
    case (LABEL):
        cp = malloc(strlen(arg) + 1);
        strcpy(cp, arg);
        tmp = strtok(cp, "[");
        if (strcmp(tmp, arg))
        {
            free(cp);
            return INDEX;
        }
        free(cp);
        return DIRECT;
    case (NONE):
        return IMMEDIATE;
    }
    return DIRECT_REGISTER;
}

/*Returns the binary of the second word of an instruction*/
int get_word_bin(Instruction *ins)
{
    int tmp = 0;
    if (ins->second_argument == NULL)
    {
        tmp |= get_funct(ins->name);
        tmp |= (get_reg(ins->first_argument) << 2);
        tmp |= (get_reg_from_label(ins->first_argument) << 2);
        tmp |= (get_addresing_mode(ins->first_argument));
    }
    else
    {
        tmp |= get_funct(ins->name);
        tmp |= (get_reg(ins->first_argument) << 8);
        if (get_reg_from_label(ins->first_argument) == NONE)
            return NONE;
        tmp |= (get_reg_from_label(ins->first_argument) << 8);
        tmp |= (get_addresing_mode(ins->first_argument) << 6);
        tmp |= (get_reg(ins->second_argument) << 2);
        if (get_reg_from_label(ins->second_argument) == NONE)
            return NONE;
        tmp |= (get_reg_from_label(ins->second_argument) << 2);
        tmp |= (get_addresing_mode(ins->second_argument));
    }
    return tmp;
}

Boolean number_check(char *arg)
{
    char cp[BUFFER_SIZE];
    char *res;
    double float_number = 0;
    strcpy(cp, arg);
    res = strtok(cp, "#");
    if (res == NULL || (atoi(res) == 0 && res[0] != '0'))
    {
        fprintf(stderr, "%s:%d: Invalid number\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    float_number = atof(res);
    if (float_number != (int)float_number)
    {
        fprintf(stderr, "%s:%d: Invalid number; only integers supported\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    return TRUE;
}

/*Returns the number from the given argument*/
int get_number_arg(char *arg)
{
    char cp[BUFFER_SIZE];
    char *res;
    int num;
    strcpy(cp, arg);
    res = strtok(cp, "#");
    if (res != NULL && (atoi(res) != 0 || res[0] == '0'))
    {
        num = atoi(res);
        return (num & ARE_CLEAR) | ABSOLUTE;
    }
    return NONE;
}

/*Builds instruction with one arguments (label)*/
int build_label_only_ins(Instruction *instr)
{
    Label *l1 = NULL;
    char name[BUFFER_SIZE];
    get_label_name(instr->first_argument, name);
    if (get_arg_type(instr->first_argument) == LABEL)
    {
        l1 = get_label(name);
    }
    else
    {
        get_label_name(instr->second_argument, name);
        l1 = get_label(name);
    }
    if (l1 == NULL)
    {
        /*The label was not found in the label table => was undefined before*/
        fprintf(stderr, "%s:%d: The label \"%s\" is undefined\n", file_name(NULL), get_line_counter(), name);
        return BUILD_FAILED;
    }
    /*If the label is external, adding it to .ext file in address of IC + 2 (opcode + funct fields)*/
    if (l1->EE == EXTERN)
        add_to_extern(l1->name, get_ic() + 2);
    instr->bin[2] = l1->base_address;
    instr->bin[3] = l1->offset;
    return BUILD_SUCCEEDED;
}

/*Builds instruction with two arguments (label and number)*/
int build_label_number_ins(Instruction *instr)
{
    Label *l1;
    char name[BUFFER_SIZE];
    if (get_arg_type(instr->first_argument) == NUMBER)
    {
        /*Expecting first argument as number and the second as label*/
        get_label_name(instr->second_argument, name);
        if (!(l1 = get_label(name)))
        {
            fprintf(stderr, "%s:%d: The label \"%s\" is undefined\n", file_name(NULL), get_line_counter(), name);
            return BUILD_FAILED;
        }
        if (!number_check(instr->first_argument))
            return BUILD_FAILED;
        instr->bin[2] = get_number_arg(instr->first_argument);
        if (l1->EE == EXTERN)
            add_to_extern(l1->name, get_ic() + 3);
        instr->bin[3] = l1->base_address;
        instr->bin[4] = l1->offset;
    }
    else
    {
        /*Expecting first argument as label and the second as number*/
        get_label_name(instr->first_argument, name);
        if (!(l1 = get_label(name)))
        {
            fprintf(stderr, "%s:%d: The label \"%s\" is undefined\n", file_name(NULL), get_line_counter(), name);
            set_errors_seen(SEEN);
            return BUILD_FAILED;
        }
        if (l1->EE == EXTERN)
            add_to_extern(l1->name, get_ic() + 2);
        instr->bin[2] = l1->base_address;
        instr->bin[3] = l1->offset;
        if (!number_check(instr->second_argument))
            return BUILD_FAILED;
        instr->bin[4] = (get_number_arg(instr->second_argument) & ARE_CLEAR) | ABSOLUTE;
    }
    return BUILD_SUCCEEDED;
}

/*Builds instrution with two label arguments*/
int build_label_label_ins(Instruction *instr)
{
    Label *l1, *l2;
    char name1[BUFFER_SIZE];
    char name2[BUFFER_SIZE];
    get_label_name(instr->first_argument, name1);
    get_label_name(instr->second_argument, name2);
    if (!(l1 = get_label(name1)))
    {
        fprintf(stderr, "%s:%d: The label \"%s\" is undefined\n", file_name(NULL), get_line_counter(), name1);
        return BUILD_FAILED;
    }
    if (!(l2 = get_label(name2)))
    {
        fprintf(stderr, "%s:%d: The label \"%s\" is undefined\n", file_name(NULL), get_line_counter(), name2);
        return BUILD_FAILED;
    }
    /*Both arguments are defined labels*/
    if (l1->EE == EXTERN)
        add_to_extern(l1->name, get_ic() + 2); /*Check if the labels are external*/
    if (l2->EE == EXTERN)
        add_to_extern(l2->name, get_ic() + 4);
    instr->bin[2] = l1->base_address;
    instr->bin[3] = l1->offset;
    instr->bin[4] = l2->base_address;
    instr->bin[5] = l2->offset;
    return BUILD_SUCCEEDED;
}

Boolean ins_structure_check(char *line)
{
    char cp[BUFFER_SIZE];
    char *word = NULL;
    if (!comma_check(line, INSTRUCTION))
    {
        fprintf(stderr, "%s:%d: Invalid commas, can't create instruction\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    strcpy(cp, line);
    strtok(cp, " ");
    word = strtok(NULL, " ");
    if (word && *word == ',')
    {
        fprintf(stderr, "%s:%d: Invalid commas, can't create instruction\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    strcpy(cp, line);
    strtok(cp, " \n");
    strtok(NULL, ",");
    strtok(NULL, " ,");
    if (strtok(NULL, ", ") != NULL)
    {
        fprintf(stderr, "%s:%d: Invalid text after end of instruction\n", file_name(NULL), get_line_counter());
        set_errors_seen(SEEN);
        return FALSE;
    }
    return TRUE;
}

/*Executes instruction
 */
int build_instruction(char *line)
{
    Instruction instr;
    char cp[BUFFER_SIZE];
    int size;
    int ptr;
    if (!ins_structure_check(line))
        return BUILD_FAILED;
    strcpy(cp, line);
    fill_instruction(&instr, cp); /*Passing copy of line since fill_instruction modifies the input*/
    if (!check_ins_args(&instr))
        return BUILD_FAILED;
    size = get_ins_size(&instr); /*Calculating the number of required words*/
    instr.bin[0] |= get_opcode(instr.name);
    if (instr.name == STOP || instr.name == RTS)
        size = 1;
    instr.bin[1] |= get_word_bin(&instr);
    switch (size)
    {
    case (NO_ARGUMENTS):
    case (SIMPLE_ARGS):
        break;
    case (NUMBER_ONLY):
        if (!number_check(instr.first_argument))
            return BUILD_FAILED;
        instr.bin[2] = get_number_arg(instr.first_argument) | ABSOLUTE;
        break;
    case (LABEL_ONLY):
        if (build_label_only_ins(&instr) == BUILD_FAILED)
        {
            set_errors_seen(SEEN);
            return BUILD_FAILED;
        }
        break;
    case (LABEL_NUMBER):
        if (build_label_number_ins(&instr) == BUILD_FAILED)
        {
            set_errors_seen(SEEN);
            return BUILD_FAILED;
        }
        break;
    case (LABEL_LABEL):
        if (build_label_label_ins(&instr) == BUILD_FAILED)
        {
            set_errors_seen(SEEN);
            return BUILD_FAILED;
        }
        break;
    default:
        fprintf(stderr, "%s:%d: Invalid instruction size of words: %d\n", file_name(NULL), get_line_counter(), size);
        set_errors_seen(SEEN);
        return BUILD_FAILED;
    }
    ptr = allocate_instruction(instr.bin, size);
    return ptr;
}
