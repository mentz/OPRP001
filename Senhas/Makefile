# GNU Makefile

CC = g++
LDFLAGS = -lm -lcrypt -fopenmp -std=c++11
CFLAGS = -Ofast
DEBFLAGS = -g -pg

openmp: src/quebra.cpp
	$(CC) -o quebra $^ $(CFLAGS) $(LDFLAGS)

mpi: src/quebra_mpi.cpp
	mpic++ -o quebra_mpi $^ $(LDFLAGS) $(DEBFLAGS)

teste: src/teste_wordgen.cpp
	$(CC) -o $@ $^ $(LDFLAGS) $(DEBFLAGS)

debug: $(FILES)
	$(CC) -o $(TARGET) $(FILES) $(LDFLAGS) $(DEBFLAGS)

# Dependencias

clean:
	rm -f gmon.out main quebra quebra_mpi

.PHONY: openmp mpi debug clean
