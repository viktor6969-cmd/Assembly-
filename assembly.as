; file ps.as
.entry LIST
.extern W
.define sz = 2
MAIN: mov r3, LIST[12]
LOOP: jmp W
 prn #-5
 mov STR[5], STR[t]
 sub r1, r4 df
 cmp K, #sz
 bne W
L1: inc L3
.entry LOOP
 bne LOOP
END: hlt
.define len = 4
STR: .string “abcdef”
LIST: .data 6, -9, len
K: .data 22
.extern L3 