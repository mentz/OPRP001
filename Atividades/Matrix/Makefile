# GNU Makefile

CC = gcc
CCFLAGS = -Wall
LDFLAGS =
TARGET = main

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

%: %.o
	$(CC) $(LDFLAGS) $^ -o $@

all: $(TARGET)

# Dependencias

main: matrix.o utils.o main.c
matrix.o: matrix.c matrix.h
utils.o: utils.c utils.h

clean:
	rm -f *.o *~ $(TARGET)
