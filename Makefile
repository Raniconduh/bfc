SOURCES = src/*.c
INCLUDEDIR = include
CFLAGS = -Wall -Wextra -I$(INCLUDEDIR)
DEST = bfc
CC ?= cc
RM ?= rm -f

.PHONY: x86_64 amd64 arm64 aarch64 clean

x86_64: amd64

amd64:
	$(CC) $(CFLAGS) -D$@ -o $(DEST) $(SOURCES)

arm64: aarch64

aarch64:
	$(CC) $(CFLAGS) -D$@ -o $(DEST) $(SOURCES)

clean:
	$(RM) $(DEST)
