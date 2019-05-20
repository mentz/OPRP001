# GNU Makefile

CC = nvcc
LDFLAGS = -lm
DEBFLAGS = -g -pg
TARGET = main
FILES = src/matrix.cu src/utils.cu src/main.cu

all: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(LDFLAGS)

debug: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(LDFLAGS) $(DEBFLAGS)

# Dependencias

clean:
	rm -f gmon.out main

.PHONY: all debug clean
