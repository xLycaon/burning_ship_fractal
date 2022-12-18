CPP = cpp
CC = gcc-12
CPPFLAGS = -std=c17 -pedantic
CFLAGS = -O0 -std=c17 -Wall -Wextra -Wpedantic -g

SRCDIR = src
OUT = prog

SRC = $(wildcard $(SRCDIR)/*.c)

.PHONY: clean run all debug

all: $(OUT)
$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(OUT)

run:
	./$(OUT) -h

debug:
	gdb $(OUT)
