# GNU Makefile

CC = g++
LDFLAGS = -lm -lcrypt -fopenmp -std=c++11
CFLAGS = -Ofast
DEBFLAGS = -g -pg

quebra: src/quebra.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

quebra_mpi: src/quebra_mpi.cpp
	mpic++ -o $@ $^ $(LDFLAGS) $(DEBFLAGS)

teste: src/teste_wordgen.cpp
	$(CC) -o $@ $^ $(LDFLAGS) $(DEBFLAGS)

debug: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(LDFLAGS) $(DEBFLAGS)

# Dependencias

clean:
	rm -f gmon.out main

.PHONY: openmp mpi debug clean
