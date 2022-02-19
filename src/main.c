#include <stdio.h>
#include <stdlib.h>

#define FPUTS(s, f) fputs(s, f); fputc('\n', f)

#define STACK "stack"
#define STACK_SIZE "256"
#define NL "\n"

#if defined(arm64) || defined(aarch64)
#define DATA ".data"
#define TEXT ".text"
#define GLOBAL(VAR) ".globl " VAR
#define PTR_REG "x11"
#define W_REG "w10"
#define LBL_MAIN "_start"
#define SYS_CALL "svc #0"

#define ADD(X, N) "add " X "," X "," "#" #N
#define SUB(X, N) "sub " X "," X "," "#" #N
//#define INC(X) ADD(X, 1)
//#define DEC(X) SUB(X, 1)
#define LDR(A, B) "ldrb " A "," B
#define STR(A, B) "strb " A "," B

#define VAR_REF "="
#define DEREF(REG) "[" REG "]"
#define FILL(N) ".zero " N

#define MKE_STACK STACK ": " FILL(STACK_SIZE)
#define INIT LBL_MAIN ":" NL "ldr " PTR_REG ", " VAR_REF STACK

//#define INCPTR ADD(PTR_REG, "1")
//#define DECPTR SUB(PTR_REG, "1")
//#define INCVAL LDR(W_REG, DEREF(PTR_REG)) NL INC(W_REG) NL STR(W_REG, DEREF(PTR_REG))
//#define DECVAL LDR(W_REG, DEREF(PTR_REG)) NL DEC(W_REG) NL STR(W_REG, DEREF(PTR_REG))
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
#endif // arm64 || aarch6

#define HD_TEXT DATA NL MKE_STACK NL TEXT NL GLOBAL(LBL_MAIN) NL INIT

int loops = 0;
int labels[128] = {0};
int * labels_ptr = labels;

struct op {
	char operator;
	int count;
	struct op * next;
};

void die(char * s) {
	FPUTS(s, stderr);
	exit(1);
}

void help(void) {
	puts(
			"bfc: A brainfuck compiler\n"
			"Usage:\n"
			"  -o <file>      The file to output to (outputs assembly)\n"
			"  --help         Show this screen and exit\n"
			"  <file>         A file not accompanied with a flag will be the input brainfuck file\n"
			"Example:\n"
			"  $ bfc input.bf -o output.asm\n"
			"  $ as -o output.o output.asm\n"
			"  $ ld -o output output.o\n"
			"  $ ./output"
		);
	exit(0);
}

struct op * new_op(void) {
	struct op * n = malloc(sizeof(struct op) + 8);
	n->next = NULL;
	n->operator = 0;
	n->count = 0;
	return n;
}

int main(int argc, char ** argv) {
	if (argc < 2) die("bfc: not enough arguments");

	char * output_path = NULL;
	char * input_path = NULL;

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-o") && i + 1 < argc) output_path = argv[++i];
		else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) help();
		else input_path = argv[i];
	}

	FILE * input_file = fopen(input_path, "r");
	FILE * output_file = fopen(output_path, "w");

	if (!input_file || !output_file)
		die("bfc: could not open either input or output files");
	
	FPUTS(HD_TEXT, output_file);

	char i_buf[sizeof(int) + 8];

	struct op * ops = new_op();
	struct op * n = ops;

	n->next = new_op();

	int c;
	while ((c = fgetc(input_file)) != EOF) {
		switch(c) {
			case '+':
			case '-':
			case '>':
			case '<':
			case '.':
			case ',':
				if (n->operator == c) n->count++;
				else {
					n->next = new_op();
					n->next->operator = c;
					n = n->next;
				}
				break;
			case '[':
			case ']':
				n->next = new_op();
				n->next->operator = c;
				n = n->next;
				break;
			default:
				break;
		}
	}

	fclose(input_file);

	n = ops->next;
	free(ops);
	ops = n;

	for (struct op * o = ops; o; o = o->next) {
		switch (o->operator) {
			case '+':
				fprintf(output_file, BFADDV, o->count + 1);
				break;
			case '-':
				fprintf(output_file, BFSUBV, o->count + 1);
				break;
			case '>':
				fprintf(output_file, BFADDP, o->count + 1);
				break;
			case '<':
				fprintf(output_file, BFSUBP, o->count + 1);
				break;
			case '.':
				FPUTS(BFPUTCH, output_file);
				for (int i = 0; i < o->count + 1; i++) {
					FPUTS(SYS_CALL, output_file);
				}
				break;
			case ',':
				FPUTS(BFGETCH, output_file);
				for (int i = 0; i < o->count + 1; i++) {
					FPUTS(SYS_CALL, output_file);
				}
				break;
			case '[':
				sprintf(i_buf, "loop_%d:", loops + 1);
				FPUTS(i_buf, output_file);
				*labels_ptr++ = loops;
				loops++;
				break;
			case ']':
				labels_ptr--;
				fprintf(output_file, BFJMPLBL, *labels_ptr + 1);
				break;
			default: break;
		}
	}

	FPUTS(END_TEXT, output_file);

	fclose(output_file);

	for (struct op * o = ops; o; o = o ->next) {
		free(o);
	}

	return 0;
}