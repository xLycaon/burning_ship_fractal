CC = gcc-12
CFLAGS = -O0 -std=c17 -Wall -Wextra -Wpedantic -g -I$(INCLDIR) -D_POSIX_C_SOURCE -D_XOPEN_SOURCE

SRCDIR = src
INCLDIR = incl
OUT = prog

SRC = $(wildcard $(SRCDIR)/*.c)

.PHONY: clean run all debug

all: $(OUT)
$(OUT): $(SRC) 
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(OUT)
	rm -f *.bmp

run:
	./$(OUT) -h

debug:
	gdb $(OUT)
