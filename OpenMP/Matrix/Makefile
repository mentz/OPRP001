# GNU Makefile

CC = g++
LDFLAGS = -Wall -pthread -lm -fopenmp -std=c++11
DEBFLAGS = -g -pg
TARGET = main
FILES = src/matrix.cpp src/utils.cpp src/main.cpp

all: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(LDFLAGS)

debug: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(LDFLAGS) $(DEBFLAGS)

# Dependencias

clean:
	rm -f gmon.out main

.PHONY: all debug clean
