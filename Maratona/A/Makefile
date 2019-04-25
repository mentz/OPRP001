FLAGS=-O3 -std=c++11 -Wall -fopenmp
RM=rm -f
CC=g++

EXEC=paralelo
SERIAL=serial

all: $(EXEC) $(SERIAL)

$(EXEC):
	$(CC) $(FLAGS) $(EXEC).cpp -o $(EXEC) -lm

$(SERIAL):
	$(CC) $(FLAGS) $(SERIAL).cpp -o $(SERIAL) -lm

clean:
	$(RM) $(EXEC).o $(EXEC) $(SERIAL).o $(SERIAL) saidap saidas

.PHONY: all $(EXEC)