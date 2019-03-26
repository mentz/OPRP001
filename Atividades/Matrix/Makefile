# GNU Makefile

CC = gcc
LDFLAGS = -Wall -pthread -lm
TARGET = main
FILES = src/matrix.c src/utils.c src/main.c

all: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(LDFLAGS)

# Dependencias

clean:
	rm -f $(TARGET)

.PHONY: all clean