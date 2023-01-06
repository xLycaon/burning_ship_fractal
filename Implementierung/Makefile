CC = gcc-12
CFLAGS = -O0 -std=c17 -Wall -Wextra -Wpedantic -g -I$(INCLDIR) -D_POSIX_C_SOURCE=200809L
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
	rm -r *.dSYM

run:
	./$(OUT)

debug:
	gdb $(OUT)
