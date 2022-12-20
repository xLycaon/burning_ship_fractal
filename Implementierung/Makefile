CPP = cpp
CC = gcc-12
CPPFLAGS = -std=c17 -pedantic
CFLAGS = -O0 -std=c17 -Wall -Wextra -Wpedantic -g -I$(INCLDIR)

SRCDIR = src
INCLDIR = incl
OUT = prog

SRC = $(wildcard $(SRCDIR)/*.c)
#INCL = $(wildcard $(INCLDIR)/*.h)

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
