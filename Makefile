

SRC = ../src
PRE_LINK_FLAGS = -g -Wall -pedantic -ansi -c

assembler: main.o assembler.o utl.o line_check.o deractives.o labels.o instructions.o macro_builder.o
	gcc -o assembler main.o assembler.o utl.o line_check.o deractives.o labels.o instructions.o macro_builder.o -lm

main.o: $(SRC)/main.c
	gcc $(PRE_LINK_FLAGS) $(SRC)/main.c

assembler.o: $(SRC)/assembler.c
	gcc $(PRE_LINK_FLAGS) $(SRC)/assembler.c
	
utl.o: $(SRC)/utl.c 
	gcc $(PRE_LINK_FLAGS) $(SRC)/utl.c

line_check.o: $(SRC)/line_check.c
	gcc $(PRE_LINK_FLAGS) $(SRC)/line_check.c

deractives.o: $(SRC)/deractives.c
	gcc $(PRE_LINK_FLAGS) $(SRC)/deractives.c

labels.o: $(SRC)/labels.c
	gcc $(PRE_LINK_FLAGS) $(SRC)/labels.c

instructions.o: $(SRC)/instructions.c
	gcc $(PRE_LINK_FLAGS) $(SRC)/instructions.c

macro_builder.o: $(SRC)/macro_builder.c
	gcc $(PRE_LINK_FLAGS) $(SRC)/macro_builder.c
