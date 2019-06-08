# GNU Makefile

CC = g++
FLAGS = -lm -lcrypt -fopenmp -std=c++11
OPTIMIZATION = -Ofast
DEBFLAGS = -g -pg

quebra: src/quebra.cpp
	mpic++ -o $@ $^ $(OPTIMIZATION) $(FLAGS)

teste: src/teste_wordgen.cpp
	$(CC) -o $@ $^ $(OPTIMIZATION) $(FLAGS)

debug: src/quebra.cpp
	$(CC) -o $@ $^ $(FLAGS) $(DEBFLAGS)

# Dependencias

clean:
	rm -f gmon.out main quebra

.PHONY: quebra teste debug clean
