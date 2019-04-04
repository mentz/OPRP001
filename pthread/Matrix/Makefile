# GNU Makefile

CC = gcc
LDFLAGS = -Wall -pthread -lm
DEBFLAGS = -g -pg
TARGET = main
FILES = src/matrix.c src/utils.c src/main.c

all: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(LDFLAGS)

debug: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(LDFLAGS) $(DEBFLAGS)

# Dependencias

clean:
	rm -f gmon.out main

.PHONY: all debug clean
