; file ps.as
.entry LIST
STR: .string “abcdef”
.extern W
.define sz = 2
MAIN: mov r3, LIST[sz]
LOOP: jmp W
 prn #-5
 mov STR[5], STR[2]
 sub r1, r4
 cmp K, #sz
 jmp LOOP
L1: inc L3
.entry LOOP
 bne LOOP
END: hlt
.define len = 4
LIST: .data 6, -9, len
K: .data 22
.extern L3 