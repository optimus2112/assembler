;input 1
;instruction errors
label: unrec #34, r10
multCommas: add #34, ,r10
missingComma: add #34 r10
notFoundFirst: jmp
notFoundSec: add #34
notFoundSec2: add #34    ,
invldNum: add #10.2, r10
invldReg: add #-6, symb[xyz]
toolongsymbolabcdefghijklmnopqrstuvwxyz: rts
illegal_symb: jmp symb
0strisawesome: rts
r10: add #-6, r10
add: add #-6, r10
Add: add #-6, r10
macro: add #-6, r10
endm: add #-6, r10
jmp symb]r10[
jmp symb[r16]
add #34, #22
jmp #55
add #-6, r10,
add #-6, r10    ,
add #-6, r10    hey!
