# GNU Makefile

CC = gcc
CCFLAGS = -Wall
LDFLAGS = -pthread
TARGET = main

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

%: %.o
	$(CC) $(LDFLAGS) $^ -o $@

all: $(TARGET)
.PHONY: all

# Dependencias
main: matrix.o utils.o main.c
matrix.o: matrix.c matrix.h
utils.o: utils.c utils.h

clean:
	rm -f *.o *~ *.out $(TARGET)
