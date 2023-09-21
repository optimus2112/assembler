;input 2
String1: .string "Hello, World! (*)"
Numbers1: .data -1, 1, -2, 78, 90, 4321, -345
WARNING: .extern externLab
Zeros: .data 0,     0,  0   , 0 ,   0
.entry Zeros
CIsCool: .string "Because of pointers"
.entry CIsCool
bne Zeros
jsr CIsCool
red r4
red CIsCool
red Numbers1[r12]
prn r9
prn #48
prn TheAbc
rts
rts
stop
TheAbc: .string "abcdefghijklmnopqrstuvqxyz"
.data 1,2,3,4,5,6
.string "hello World!!!"
WARN: .entry TheAbc