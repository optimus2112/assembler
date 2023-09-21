;input 1
String1: .string "Hello, World! (*)"
Numbers1: .data -1, 1, -2, 78, 90, 4321, -345
WARNING: .extern externLab
Zeros: .data 0,     0,  0   , 0 ,   0
.entry Zeros
mov #0, Numbers1
mov #-1, r5
mov r5, Numbers1
mov r5, r3
mov Zeros, Numbers1
mov #2, r10
mov r10, externLab
mov #4, r12
mov Numbers1[r10], Zeros
cmp #-1, r5
cmp r5, r3
cmp r5, Numbers1
cmp Numbers1[r10], Zeros
cmp Zeros, r5
add #3957, Numbers1[r12]
add r7, String1
add String1[r10], r8
sub r8, r3
sub String1[r12], Numbers1
lea Zeros, r0
lea Numbers1[r12], r1
lea String1, Zeros
clr LongSymbol1234567
not r5
not r3
inc r1
inc Numbers1
inc Zeros
dec String1[r12]
Code: add #-48, r3
jmp Code
LongSymbol1234567: .string "ILikeC"