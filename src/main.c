#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

#if defined(arm64) || defined(aarch64)
#include "asm/arm64.h"
#elif defined(amd64) || defined(x86_64)
#include "asm/amd64.h"
#endif


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
	struct op * n = malloc(sizeof(struct op));
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
				if (CAN_MULTI_CALL) FPUTS(BFPUTCH, output_file);
				for (int i = 0; i < o->count + 1; i++) {
					if (!CAN_MULTI_CALL) FPUTS(BFPUTCH, output_file);
					FPUTS(SYS_CALL, output_file);
				}
				break;
			case ',':
				if (CAN_MULTI_CALL) FPUTS(BFGETCH, output_file);
				for (int i = 0; i < o->count + 1; i++) {
					if (!CAN_MULTI_CALL) FPUTS(BFGETCH, output_file);
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

	struct op * o = ops;
	while (o) {
		struct op *tmp = o;
		o = o->next;
		free(tmp);
	}

	return 0;
}
