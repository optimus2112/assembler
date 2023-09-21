/*assembler.c file handels all the main assembler operation the actions, such as:
memory allocation, creating output files*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "utl.h"
#include "assembler.h"
#include "line_check.h"
#include "instructions.h"
#include "labels.h"
#define BUFFER_SIZE 81

/*Returns string representation of number in base of 2*/
void itob(int n, char* bin) {
    int i = 0;
    if (bin == NULL) {
        fprintf(stderr, "Null pointer has been given to itob function\n");
        return;
    }
    for (i = WORD_SIZE - 1; i >= 0; --i) {
        bin[i] = n%2 + 48;
        n /= 2;
    }
    bin[WORD_SIZE] = '\0';
}
void to_hex(int n, char* s) {
    int i = 0;
    int size = 0;
    int tmp = n;
    static char* hex = "0123456789abcdef";
    while (tmp/16) {
        ++size;
        tmp /= 16;
    }
    for (i = size; i >= 0; --i) {
        s[i] = hex[n%16];
        n /= 16;
    }
    s[size + 1] = '\0';
}

/*Returns register number of the given register*/
int get_reg(char* reg) {
    if (reg == NULL) return R0;
    if (!strcmp(reg, "r0")) return R0;
    if (!strcmp(reg, "r1")) return R1;
    if (!strcmp(reg, "r2")) return R2;
    if (!strcmp(reg, "r3")) return R3;
    if (!strcmp(reg, "r4")) return R4;
    if (!strcmp(reg, "r5")) return R5;
    if (!strcmp(reg, "r6")) return R6;
    if (!strcmp(reg, "r7")) return R7;
    if (!strcmp(reg, "r8")) return R8;
    if (!strcmp(reg, "r9")) return R9;
    if (!strcmp(reg, "r10")) return R10;
    if (!strcmp(reg, "r11")) return R11;
    if (!strcmp(reg, "r12")) return R12;
    if (!strcmp(reg, "r13")) return R13;
    if (!strcmp(reg, "r14")) return R14;
    if (!strcmp(reg, "r15")) return R15;
    return R0;
}

/*allocates memory in data image*/
int allocate_data(int* array, int size) {
    int i;
    int ptr = get_dc();
    if (get_ic() >= DATA_IMG_SIZE) {
        fprintf(stderr, "Not enough space for data allocation\n");
        exit(0);
    }
    for (i = 0; i < size; ++i) {
        data_img()[get_dc()] = array[i];
        inc_dc(1);
    }
    return ptr;
}

/*allcates memory in instruction image*/
int allocate_instruction(int* array, int size) {
    int i;
    int ptr = get_ic();
    if (get_ic() >= INSTRARTIONS_IMG_SIZE) {
        fprintf(stderr, "Not enough space for instruction allocation\n");
        exit(0);
    }
    for (i = 0; i < size; ++i) {
        instructions_img()[get_ic()] = array[i];
        inc_ic(1);
    }
    return ptr;
}

/*Data image array*/
int* data_img() {
    static int data_table[DATA_IMG_SIZE] = {0};
    return data_table;
}

/*instruction image array*/
int* instructions_img() {
    static int inst_table[INSTRARTIONS_IMG_SIZE] = {0};
    return inst_table;
}

/*Data counter. If action is INC (NULL) increments DC
otherwise, changes the value of the pointer to DC*/
void dc(int* action) {
    static int DC = 0;
    if (action == INCREMENT) ++DC;
    else if(*action == DECREMENT) --DC;
    else *action = DC;
}

void reset_dc() {
    int dec = DECREMENT;
    while (get_dc() > 0) dc(&dec);
}


/*Instructions counter. If action is INC (NULL) increments IC
otherwise, changes the value of the pointer to IC*/
void ic(int* action) {
    static int IC = 100;
    if (action == INCREMENT) ++IC;
    else if(*action == DECREMENT) --IC;
    else *action = IC;
}

/*Returns data counter*/
int get_dc() {
    int p = 0;
    dc(&p);
    return p;
}

/*Incremenate data counter in number of words*/
void inc_dc(int words) {
    int i;
    for (i = 0; i < words; ++i) {
        dc(INCREMENT);
    }
}


/*Returns instruction counter*/
int get_ic() {
    int p = 0;
    ic(&p);
    return p;
}

/*Incremenate instruction counter in number of word*/
void inc_ic(int words) {
    int i;
    for (i = 0; i < words; ++i) {
        ic(INCREMENT);
    }
}

/*Resets instruction counter*/
void reset_ic() {
    static int p = -1;
    while (get_ic() != 100) ic(&p);
}

/*Assembler status which indicates in which run the assembler is at*/
int status(int* action) {
    static int status = FIRST_PASS;
    if (action != NULL) status = *action;
    else return status;
    return 1;
}

/*Changes the assembler status*/
void set_status(int s) {
    status(&s);
}

/*Returns assembler status*/
int get_status() {
    return status(NULL);
}

/*Line counter*/
int line_counter(int* action) {
    static int counter = 1;
    if (action != NULL && *action == 1) counter++;
    else if(action != NULL && *action == 0) counter = 1;
    else return counter;
    return 1;
}

/*Incremenate line counter*/
void inc_line_counter() {
    int p = 1;
    line_counter(&p);
}

/*resets line counter*/
void reset_line_counter() {
    int p = 0;
    line_counter(&p);
}

/*Returns line counter*/
int get_line_counter() {
    return line_counter(NULL);
}

/*Prints data image*/
void print_data_img() {
    int i = 0;
    char buffer[21] = {0};
    while (i < 30) {
        strcpy(buffer, "");
        itob(data_img()[i++], buffer);
        printf("%s\n", buffer);

    }
}

/*Prints Instruction image*/
void print_ins_img() {
    int i = 100;
    char buffer[21];
    while (i < 120) {
        itob(instructions_img()[i++], buffer);
        printf("%s\n", buffer);
    }
}

/*Handles the file name;
if given arguments is not NULL, applays this name as file name, otherwise returns the current fil name*/
char* file_name(char* name) {
    static char* name_ptr = NULL;
    if (name != NULL) {
        name_ptr = name; 
    }
    return name_ptr;
}

/*Adds label to entry file*/
void add_to_entry(Label* l) {
    char* ent;
    FILE* file;
    int len;
    if (file_name(NULL) == NULL) return;
    len = strlen(file_name(NULL));
    ent = malloc(len + 5);
    strcpy(ent, file_name(NULL));
    strcat(ent, ".ent");
    if (is_ent_created(GET) == FALSE) {
        /*The file has not been created before for current input file*/
        is_ent_created(TRUE);
        file = fopen(ent, "w");/*overwriting The file .ent because it may
                                have irrelevant content from prevues runs*/
    } else file = fopen(ent, "a");
    if (file == NULL) {
        fprintf(stderr, "Can't %s open entries file\n", file_name(NULL));
        return;
    }
    fprintf(file, "%s,%d,%d\n", l->name, l->base_address & ARE_CLEAR,l->offset & ARE_CLEAR);
    free(ent);
    fclose(file);
}

/*Adds label to external file*/
void add_to_extern(char* name, int ptr) {
    char* ext;
    FILE* file;
    if (get_errors_seen() == SEEN) return;
    if (file_name(NULL) == NULL) return;
    ext = calloc(strlen(file_name(NULL)) + sizeof(".ext") + 1, 1);
    strcpy(ext, file_name(NULL));
    strcat(ext, ".ext");
    if (is_ext_created(GET) == FALSE) {
        /*The file has not been created before for current input file*/
        is_ext_created(TRUE);
        file = fopen(ext, "w"); /*overwriting The file .ext because it may
                                have irrelevant content from prevues runs*/
    } else file = fopen(ext, "a");
    if (file == NULL) {
        fprintf(stderr, "%s cannot be open\n",ext);
        return;
    }
    fprintf(file, "%s BASE %d\n%s OFFSET %d\n\n", name, ptr,name,ptr + 1);
    fclose(file);
    free(ext);
}

Boolean is_ent_created(int mode) {
    static Boolean is_created = FALSE;
    if (mode == GET) return is_created;
    is_created = mode;
    return -1;
}

Boolean is_ext_created(int mode) {
    static Boolean is_created = FALSE;
    if (mode == GET) return is_created;
    is_created = mode;
    return -1;
}

/* The first pass over the source code.
 * Gets the total instruction image size.
 * Builds directives */
void first_pass(FILE* inputfile) {
    int id;
    char cp[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    while(fgets(buffer, BUFFER_SIZE,inputfile) != NULL) {
        if (strlen(buffer) >= BUFFER_SIZE) {
            fprintf(stderr, "%s:%d: Invalid line size, maximum length expected: 80\n", file_name(NULL), get_line_counter());
            set_errors_seen(SEEN);
            goto cont;
        }
        id = line_builder(buffer, GET_TYPE);
        if (id == DATA || id == STRING || id == LABEL || id == EXTERN) {
        line_builder(buffer, EXECUTE);
        }
        if (id == INSTRUCTION) {
            Instruction tmp;
            strcpy(cp, buffer);
            fill_instruction(&tmp, cp);
            inc_ic(get_ins_size(&tmp));
        }
        if (id == NONE) {
            fprintf(stderr, "%s:%d: Undefined command\n", file_name(NULL), get_line_counter());
            set_errors_seen(SEEN);
            goto cont;
        }

        cont:
            inc_line_counter();
    }   
}

/*assembler status which indicates if the assembler is allowed to preduce object files*/
int errors_seen(int* s) {
    static int can_out = NOT_SEEN;
    if (s != NULL) can_out = *s;
    else return can_out;
    return 1; 
}

int get_errors_seen() {/*Change name*/
    return errors_seen(NULL);
}

void set_errors_seen(int s) {
    errors_seen(&s);
}

/*Second pass on the source code, builds instruction and ENTRY/EXTERN deractives*/
void second_pass(FILE* inputfile) {
    int id;
    char buffer[BUFFER_SIZE];
    while(fgets(buffer, BUFFER_SIZE,inputfile) != NULL) {
        id = line_builder(buffer, GET_TYPE);
        if (id == INSTRUCTION || id == ENTRY || id == LABEL) {
            line_builder(buffer, EXECUTE);
        }
        if (id == NONE) {
            fprintf(stderr, "%s:%d: Undefined command\n", file_name(NULL), get_line_counter());
            set_errors_seen(SEEN);
        }
        inc_line_counter();
    }
    
}

/*Updates the label address in instruction image*/
void update_label_table() {
    int i;
    int new_addres;
    for (i = 0; i < l_table()->capacity; ++i) {
        if (l_table()->array[i]->attribute == DATA || l_table()->array[i]->attribute == STRING) {
            new_addres = (l_table()->array[i]->base_address + l_table()->array[i]->offset + get_ic()) & ARE_CLEAR;
            l_table()->array[i]->base_address = (new_addres - new_addres%16) | RELOCATEBLE;
            l_table()->array[i]->offset = (new_addres%16) | RELOCATEBLE;
        }
    }
}

void print4digits(FILE* file, int num) {
    int four = num % 10;
    int three;
    int two;
    num /= 10;
    three = num %10;
    num /= 10;
    two = num %10;
    num /= 10;
    fprintf(file, "%d%d%d%d ", num%10, two, three, four);
}

void build_ins_img(FILE* file) {
    int i;
    char buffer[WORD_SIZE + 1];
    for(i = 100; i < get_ic(); ++i) {
        to_hex((instructions_img()[i] & A)>>16, buffer);
        print4digits(file, i);
        fprintf(file, "A%s-", buffer);
        to_hex((instructions_img()[i] & B)>>12, buffer);
        fprintf(file, "B%s-", buffer);
        to_hex((instructions_img()[i] & C)>>8, buffer);
        fprintf(file, "C%s-", buffer);
        to_hex((instructions_img()[i] & D)>>4, buffer);
        fprintf(file, "D%s-", buffer);
        to_hex((instructions_img()[i] & E), buffer);
        fprintf(file, "E%s\n", buffer);
    }
}

void build_data_img(FILE* file) {
    int i;
    char *buffer = (char*)calloc(WORD_SIZE + 1, 1);
    for(i = 0; i < get_dc(); ++i) {
        to_hex((data_img()[i] & A)>>16, buffer);
        print4digits(file, i + get_ic());
        fprintf(file, "A%s-",buffer);
        to_hex((data_img()[i] & B)>>12, buffer);
        fprintf(file, "B%s-", buffer);
        to_hex((data_img()[i] & C)>>8, buffer);
        fprintf(file, "C%s-", buffer);
        to_hex((data_img()[i] & D)>>4, buffer);
        fprintf(file, "D%s-", buffer);
        to_hex((data_img()[i] & E), buffer);
        fprintf(file, "E%s\n", buffer);
    }
    free(buffer);
}

/*Produce binary code to .ob file*/
void build_ob() {
    char* ob;
    FILE* file;
    int len;
    if (file_name(NULL) == NULL) return;
    len = strlen(file_name(NULL));
    ob = malloc(len + 4);
    strcpy(ob, file_name(NULL));
    strcat(ob, ".ob");
    file = fopen(ob, "w");
    fprintf(file, "%d %d\n", get_ic() - 100, get_dc());
    build_ins_img(file);
    build_data_img(file);
    fclose(file);
    free(ob);
}

/*Produce .ent file*/
void build_entries() {
    int i;
    for (i = 0; i < l_table()->capacity; ++i) {
        if (l_table()->array[i]->EE == ENTRY) add_to_entry(l_table()->array[i]);
    }
}

void clean() {
    set_status(FIRST_PASS);
    reset_ic();    
    reset_dc();
    set_errors_seen(NOT_SEEN);
    clean_l_table();
    reset_line_counter();
    is_ext_created(FALSE);
    is_ent_created(FALSE);
}

/*main assembler function
 * Assembler function pass on source code twice:
 * First time to build directives and get the total size of instruction image
 * Second time to build instructions
 * If Errors ocurred in source code, does not build object files*/
void assembler(FILE* inputfile) {
    first_pass(inputfile);
    update_label_table(); /*Now instruction img size known, updating data labels*/
    reset_ic(); /*Reset instruction counter*/
    set_status(SECOND_PASS);
    fseek(inputfile, 0, SEEK_SET); /*Reset the file pointer*/
    reset_line_counter(); /*Reset line counter*/
    second_pass(inputfile);
    if (get_errors_seen() == SEEN) {
        fprintf(stderr,"Errors found in %s, can't build object files\n", file_name(NULL));
        goto clean_up;
    }
    build_entries();
    build_ob();

    clean_up: 
        clean();
    
}