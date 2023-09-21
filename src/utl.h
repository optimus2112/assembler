/*utl.hs*/

/*
 * utl.h contains:
 * Struture definitions 
 * Structure realated functions
 * Constant variables
 * Enums
*/

#ifndef utl_h
#define utl_h
#define WORD_SIZE 20
#define INCREMENT NULL
#define MAX_LABEL_NAME_SIZE 31
#define DECREMENT -1
#define BUFFER_SIZE 81
#define ABSOLUTE 0x40000
#define RELOCATEBLE 0x20000
#define EXTERNAL 0x10000
#define ARE_CLEAR 0xFFFF
#define BUILD_FAILED -1
#define DATA_IMG_SIZE 8000
#define INSTRARTIONS_IMG_SIZE 8000
#define NONE -1
#define BUILD_SUCCEEDED 1
typedef enum {
    FALSE,
    TRUE
} Boolean;


enum Areas {
    A = 0xF0000,
    B = 0xF000,
    C = 0xF00,
    D = 0xF0,
    E = 0xF
};

/*Command's opcodes*/
enum Opcodes {
    MOV_OPCODE,
    CMP_OPCODE,
    ADD_SUB_OPCODE,
    LEA_OPCODE = 4,
    BIT_OPERATIONS,
    JMP_BNE_JSR_OPCODE=9,
    RED_OPCODE=12,
    PRN_OPCODE,
    RTS_OPCODE,
    STOP_OPCODE
};

/*Instruction argument types*/
enum InstructionsArgumentTypes {
    NO_ARGUMENTS = 1,
    SIMPLE_ARGS ,
    NUMBER_ONLY,
    LABEL_ONLY,
    LABEL_NUMBER,
    LABEL_LABEL
};

/*Addressing modes*/
enum AddresingModes {
    IMMEDIATE,
    DIRECT,
    INDEX,
    DIRECT_REGISTER
};

/*Directive types*/
typedef enum Directives_{
    STRING = 100,
    DATA,
    ENTRY,
    EXTERN
}DirectivesTypes;

enum ErrorsSeen {
    NOT_SEEN,
    SEEN
};

/*Instruction names*/
typedef enum Instructions_{
    ADD = 200,
    MOV,
    CMP,
    SUB,
    LEA,
    CLR,
    NOT,
    INC,
    BNE,
    JMP,
    DEC,
    JSR,
    RED,
    PRN,
    RTS,
    STOP
} InstructionsTypes;

typedef enum LineTypes_ {
    LABEL = 300,
    DERACTIVE,
    REGISTER,
    INSTRUCTION,
    EMPTY_LINE,
    COMMENT,
    NUMBER,
    UNDEFINED,
    OK
}LineTypes;

enum Funct {
    ADD_CLR_JMP_FUNCT = 10,
    SUB_NOT_BNE_FUNCT,
    INC_JSR_FUNCT,
    DEC_FUNCT
};

/*command structure*/
typedef struct Instruction_ {
    InstructionsTypes name; 
    char* first_argument;
    char* second_argument;

    /*whole binary of the instruction.
     * index 0 - opcode
     * index 1 - funct , registers and addressing modes
     * indexes 2 - 5: label addresses or numbers
     * note: If not labels or addresses those indexes will be 0 */
    int bin[6]; 
} Instruction;

/*Label structure*/
typedef struct Label_ {
    char* name; 
    char* content;
    int base_address;
    int offset;
    int attribute; /*Type of label; directive, instruction*/
    int EE; /*Entry or external, defult to none (-1)*/
}Label;

typedef struct LabelTable_{
    int size; /*Total size of table*/
    int capacity; /*actual count of labels in array*/
    Label** array; /*Label array*/
}LabelTable;

enum LineCheckModes {
    GET_TYPE,
    EXECUTE
};

enum Passes {
    FIRST_PASS,
    SECOND_PASS
};

/*Registers*/
enum Registers{
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15
};

void add_to_label_table(LabelTable*, Label*);
LabelTable* create_l_t();
void free_label_table();
void clean_l_table();
Instruction* create_instruction(char*);
void fill_instruction(Instruction* , char* );
void print_command(Instruction*);
Label* create_label();
void print_array(int* , int);
void change_to_obj(char**);
#endif