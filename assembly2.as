; file ps.as
.entry LIST 
gay: .extern W 
.define sz = 2 
MAIN:	mov r3, LIST[sz]
LOOP: 	jmp W 
	prn #-5
	mov STR[5], STR[r3] 
	sub r1, r
	cmp K, #sz
	bne W
L1: 	inc L3 
.entry LOOP
	bne LOOP 
END: hlt
.define len = 4
STR: .string "abcdef" 
LIST: .data 6, -5, len 
K: .data 22 
.extern L3