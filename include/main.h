#ifndef _MAIN_H
#define _MAIN_H

#define FPUTS(s, f) do { 	\
		fputs(s, f);	    \
		fputc('\n', f);	    \
	} while (0)

#endif /* _MAIN_H */
