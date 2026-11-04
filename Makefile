CC ?= cc
CFLAGS ?= -std=gnu11 -Wall -Wextra -g

.PHONY: all clean
all: build/shell

build/shell: Shell/main.c Shell/header.h
	mkdir -p build
	$(CC) $(CFLAGS) Shell/main.c -o $@

clean:
	rm -rf build
