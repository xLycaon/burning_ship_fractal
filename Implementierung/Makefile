CC = gcc-12
CFLAGS = -O2 -std=c17 -Wall -Wextra -Wpedantic -I$(INCLDIR) -D_POSIX_C_SOURCE=200809L -msse4.2 -mavx -mfma -mavx2
SRCDIR = src
INCLDIR = incl
OUT = prog

SRC = $(wildcard $(SRCDIR)/*.c)

.PHONY: cean run all debug view_asm

all: $(OUT)
$(OUT): $(SRC) 
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.out
	rm -f *.s
	rm -f $(OUT)
	rm -f *.bmp
	rm -r *.dSYM

run:
	./$(OUT) -h

debug:
	gdb $(OUT)

view_asm:
	$(CC) -msse4.2 -fverbose-asm -O2 -std=c17 -S -I$(INCLDIR) ./src/burning_ship.c