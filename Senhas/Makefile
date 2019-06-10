# GNU Makefile

CC = g++
FLAGS = -lm -lcrypt -fopenmp -pthread -std=c++11
# OPTIMIZATION = -Ofast
OPTIMIZATION = -Ofast
DEBFLAGS = -g -pg -O0

quebra: src/quebra.cpp
	mpic++ -o $@ $^ $(OPTIMIZATION) $(FLAGS)

teste: src/teste_wordgen.cpp
	$(CC) -o $@ $^ $(OPTIMIZATION) $(FLAGS)

debug: src/quebra.cpp
	mpic++ -o quebra $^ $(FLAGS) $(DEBFLAGS)

clean:
	rm -f gmon.out main quebra

.PHONY: quebra teste debug clean
