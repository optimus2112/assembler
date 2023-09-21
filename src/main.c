/*main*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utl.h"
#include "assembler.h"
#include "line_check.h"
#include "macro_builder.h"

int main(int argc, char **argv)
{
    FILE *inputfile = NULL;
    FILE *after_macro = NULL;
    char *as = NULL;
    int i = 0;
    for (i = 1; i < argc; ++i)
    {
        as = (char *)calloc(strlen(argv[i]) + sizeof(".as\0"), 1);
        strcpy(as, argv[i]);
        strcat(as, ".as");
        inputfile = fopen(as, "r");
        if (inputfile == NULL)
        {
            fprintf(stderr, "File %s cannot be opened\n", as);
            continue;
        }
        file_name(argv[i]);                  /*Set current file name*/
        after_macro = create_mac_file();     /*Creating macro file*/
        build_macro(inputfile, after_macro); /*Builds macro to macro-file*/
        fclose(inputfile);                   /*closing the old input*/
        assembler(after_macro);
        free(as);
        fclose(after_macro);
    }
    free_label_table();
    return 0;
}
