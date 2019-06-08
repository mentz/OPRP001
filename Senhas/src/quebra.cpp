#include "wordgen.h"
#include <crypt.h>
#include <iostream>
#include <map>
#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <set>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int mpi_rank;
  int mpi_size;
  MPI_Comm comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &mpi_rank);
  MPI_Comm_size(comm, &mpi_size);

  // Obter comprimento máximo
  int comprimento = 0;
  unsigned long long maximo = 64L;
  if (argc == 2) {
    sscanf(argv[1], "%d", &comprimento);
    // printf("p%d argv[1] = %s\n", mpi_rank, argv[1]);
    comprimento = MIN(8, comprimento);
    for (int i = 1; i < comprimento; i++) {
      maximo++;
      maximo *= (unsigned long long)maxSize;
    }
    // printf("p%d max = %llu\n", mpi_rank, maximo);
  } else {
    fprintf(stderr, "Falta argumento: %s <comprimento_maximo>\n", argv[0]);
    fprintf(stderr, "Uso: Informe pela entrada padrão o número de cifras e em seguida digite\n");
    fprintf(stderr, "     as cifras uma por linha.\n");
    exit(1);
  }

  // Ler senhas
  int num_cifras = 0;
  set<int> falta;
  char **cifras;
  char *cbloco;
  if (mpi_rank == 0) {
    // ROOT
    cin >> num_cifras;
    MPI_Bcast(&num_cifras, 1, MPI_INT, 0, comm);
    getchar();
    cifras = new char*[num_cifras];
    cbloco = new char[num_cifras * 32];
    string cifra;
    for (int i = 0; i < num_cifras; i++) {
      getline(cin, cifra);
      cifras[i] = &cbloco[i * 32];
      strncpy(cifras[i], cifra.data(), 16);
      falta.insert(i);
    }
    MPI_Bcast(cifras[0], num_cifras * 32, MPI_CHAR, 0, comm);
  } else {
    // Not root
    MPI_Bcast(&num_cifras, 1, MPI_INT, 0, comm);
    cifras = new char*[num_cifras];
    cbloco = new char[num_cifras * 32];
    for (int i = 0; i < num_cifras; i++) {
      cifras[i] = &cbloco[i * 32];
      falta.insert(i);
    }
    MPI_Bcast(cifras[0], num_cifras * 32, MPI_CHAR, 0, comm);
  }

  unsigned long long i = 0L, counter = 0;
#pragma omp parallel reduction(+:counter)
  {
    crypt_data myData;
    char *result = myData.crypt_3_buf;
    int inicio = (mpi_rank * omp_get_num_threads()) + omp_get_thread_num();
    // printf("p%d t%d inicia em %d\n", mpi_rank, omp_get_thread_num(), inicio);
    int passo = mpi_size * omp_get_num_threads();
    Senha senha(inicio);
    unsigned long long thread_i;
    for (thread_i = inicio; thread_i < maximo; thread_i += passo) {
      if ((thread_i % 10000) == 0) {
        if ((int)falta.size() == 0) {
          break;
        }
      }
      for (auto &e : falta) {
        result = crypt_r(senha.getSenha(), cifras[e], &myData);
        int ok = strncmp(result, cifras[e], 14) == 0;

        if (ok) {
          printf("p]t[%*d, %2.f%%] %s = %s\n", (int)ceil(log10(passo)),
                 inicio, (thread_i / (double)maximo) * 100, cifras[e],
                 senha.getSenha());
          fflush(stdout);

          if (falta.count(e) > 0)
            falta.erase(e);
        }
      }

      if (((thread_i + 1) % 100000) == 0) {
        fprintf(stderr, "Realizado %2.f%% ou %llu de %llu\n",
                (thread_i / (double)maximo) * 100, thread_i + 1, maximo);
      }

      counter++;
    }
  }
  fprintf(stderr, "[%d] terminou em %llu iterações!!!!\n", mpi_rank, counter);

  MPI_Finalize();

  return 0;
}
