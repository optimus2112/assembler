/*Instruction.c handels all instruction realated action, such as:
 * Check if the instruction is valid
 * calculate instruction size
 * add labels to extern file if used*/

#ifndef ins_h
#define ins_h
/*Builds instruction*/
int build_instruction(char*);

/*Returns the size (in words) of instruction*/
int get_ins_size(Instruction* );
#endif