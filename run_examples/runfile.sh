#bin/bash

#run file

file1="./invalid_run1/input"
file2="./invalid_run2/input"
file3="./valid_run1/input"
file4="./valid_run2/input"

#Attached run 1:
#run all file together
../bin/assembler $file1 $fil2 $file3 $file4

#Attached run2:
#run invalid file to rederect error printing
../bin/assembler $file1 &>./invalid_run1/out.txt
../bin/assembler $file2 &>./invalid_run2/out.txt