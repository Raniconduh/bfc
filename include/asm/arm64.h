#ifndef _ARM64_ASM_H
#define _ARM64_ASM_H

#include "asm/general.h"

#define PTR_REG "x11"
#define W_REG "w10"
#define LBL_MAIN "_start"
#define SYS_CALL "svc #0"

#define ADD(X, N) "add " X "," X "," "#" #N
#define SUB(X, N) "sub " X "," X "," "#" #N
#define LDR(A, B) "ldrb " A "," B
#define STR(A, B) "strb " A "," B

#define VAR_REF "="
#define DEREF(REG) "[" REG "]"
#define FILL(N) ".zero " N

#define MKE_STACK STACK ": " FILL(STACK_SIZE)
#define INIT LBL_MAIN ":" NL "ldr " PTR_REG ", " VAR_REF STACK

#define BFPUTCH "mov x0,#1" NL       \
                "mov x1," PTR_REG NL \
                "mov x2,#1" NL       \
                "mov w8,#64"

#define BFGETCH "mov x0,#1" NL       \
                "mov x1," PTR_REG NL \
                "mov x2,#1" NL       \
                "mov w8, #63"

#define BFADDV LDR(W_REG, DEREF(PTR_REG))    NL \
               "add " W_REG "," W_REG ",#%d" NL \
               STR(W_REG, DEREF(PTR_REG))    NL

#define BFSUBV LDR(W_REG, DEREF(PTR_REG))    NL \
               "sub " W_REG "," W_REG ",#%d" NL \
               STR(W_REG, DEREF(PTR_REG))   NL

#define BFADDP "add " PTR_REG "," PTR_REG ",#%d" NL
#define BFSUBP "sub " PTR_REG "," PTR_REG ",#%d" NL
#define BFJMPLBL LDR(W_REG, DEREF(PTR_REG)) NL \
                 "cbnz " W_REG ", loop_%d"  NL

#define END_TEXT "mov x0,#0"  NL \
                 "mov w8,#93" NL \
                 "svc #0"

#define HD_TEXT DATA NL MKE_STACK NL TEXT NL GLOBAL(LBL_MAIN) NL INIT

#define CAN_MULTI_CALL 1

#endif /* _ARM64_ASM_H */
