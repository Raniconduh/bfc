#ifndef _AMD64_ASM_H
#define _AMD64_ASM_H

#include "asm/general.h"

#ifdef __linux__
#define HW_WRITE "1"
#define HW_READ "0"
#define HW_EXIT "60"
#endif /* __linux__ */

#ifdef __FreeBSD__
#define HW_WRITE "4"
#define HW_READ "3"
#define HW_EXIT "1"
#endif /* __FreeBSD__ */

#define PTR_REG "r12"
#define LBL_MAIN "_start"
#define SYS_CALL "syscall"

#define REG_REF "%"
#define VAR_REF "$"

#define ADD(R, N) "add " VAR_REF #N "," #R

#define DEREF(REG) "(" REG_REF REG ")"
#define FILL(N) ".zero " N

#define MKE_STACK STACK ": " FILL(STACK_SIZE)
#define INIT LBL_MAIN ":" NL "mov " VAR_REF STACK "," REG_REF PTR_REG

#define BFPUTCH "mov $" HW_WRITE ",%rax" NL       \
                "mov $1,%rdi" NL                  \
				"mov " REG_REF PTR_REG ",%rsi" NL \
				"mov $1, %rdx"

#define BFGETCH "mov $" HW_READ ",%rax" NL        \
                "mov $0,%rdi" NL                  \
				"mov " REG_REF PTR_REG ",%rsi" NL \
				"mov $1, %rdx"

#define BFADDV "addb $%d,(%%" PTR_REG ")" NL
#define BFSUBV "subb $%d,(%%" PTR_REG ")" NL

#define BFADDP "add $%d, %%" PTR_REG NL
#define BFSUBP "sub $%d, %%" PTR_REG NL

#define BFJMPLBL "cmpb $0, (%%" PTR_REG ")" NL \
                 "jnz loop_%d" NL

#define END_TEXT "mov $" HW_EXIT ",%rax"  NL \
                 "mov $0,%rdi" NL            \
                 "syscall" NL

#define HD_TEXT DATA NL MKE_STACK NL TEXT NL GLOBAL(LBL_MAIN) NL INIT

#define CAN_MULTI_CALL 0

#endif /* _AMD64_ASM_H */
