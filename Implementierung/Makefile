CC = gcc-12
CFLAGS = -O2 -g -std=c17 $(WARNING_FLAGS) -D_POSIX_C_SOURCE=200809L -march=native #-fno-omit-frame-pointer -mno-omit-leaf-frame-pointer

WARNING_FLAGS = -Wall -Wextra -Wpedantic
SSE_FLAGS = -msse4.2
AVX_FLAGS = -mavx -mfma -mavx2

SRCDIR = src
INCLDIR = incl

OUT = prog

SRC = $(wildcard $(SRCDIR)/*.c)

.PHONY: all clean debug

all: $(OUT)

$(OUT): $(SRC) 
	$(CC) -I$(INCLDIR) $(CFLAGS) -o $@ $^

clean:
	rm -f *.out
	rm -f *.s
	rm -f $(OUT)
	rm -f *.bmp
	rm -r *.dSYM

debug:
	gdb $(OUT)
