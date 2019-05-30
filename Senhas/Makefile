# GNU Makefile

CC = g++
LDFLAGS = -lm -lcrypt -std=c++11
CFLAGS = -Ofast
DEBFLAGS = -g -pg
TARGET = main
FILES = src/quebra.cpp

all: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(CFLAGS) $(LDFLAGS)

debug: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(LDFLAGS) $(DEBFLAGS)

# Dependencias

clean:
	rm -f gmon.out main

.PHONY: all debug clean
