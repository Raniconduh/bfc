SOURCES=src/*.c
LIBS=""
INCLUDEDIR=include
CFLAGS=-Wall -Wextra -I$(INCLUDEDIR)
DEST="bfc"
CC?=cc
RM?=rm -f

.PHONY: all clean

x86_64: amd64

amd64:
	$(CC) $(CFLAGS) -Damd64 -o $(DEST) $(SOURCES) $(LIBS)

arm64: aarch64

aarch64:
	$(CC) $(CFLAGS) -Daarch64 -o $(DEST) $(SOURCES) $(LIBS)

clean:
	$(RM) $(DEST)
