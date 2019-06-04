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
    comprimento = MIN(8, comprimento);
    for (int i = 1; i < comprimento; i++) {
      maximo++;
      maximo *= (unsigned long long)maxSize;
    }
  } else {
    fprintf(stderr, "Falta argumento: %s <comprimento_maximo>\n", argv[0]);
    fprintf(stderr, "Uso: Informe pela entrada padrão o número de cifras e em seguida digite\n");
    fprintf(stderr, "     as cifras uma por linha.\n");
    exit(1);
  }

  // Ler senhas
  int num_cifras = 0;
  cin >> num_cifras;
  getchar();
  set<int> falta;
  vector<string> cifras;
  string cifra;
  for (int i = 0; i < num_cifras; i++) {
    getline(cin, cifra);
    cifras.push_back(cifra);
    falta.insert(i);
  }

  unsigned long long i = 0L;
#pragma omp parallel
  {
    crypt_data myData;
    char *result = myData.crypt_3_buf;
    int inicio = (mpi_rank * mpi_size) + omp_get_thread_num() + 1;
    int passo = mpi_size * omp_get_num_threads();
    Senha senha(inicio);
    unsigned long long thread_i, counter = 0;
    for (thread_i = inicio; thread_i < maximo; thread_i += passo) {
      if ((thread_i % 10000) == 0) {
        if ((int)falta.size() == 0) {
          break;
        }
      }
      for (auto &e : falta) {
        result = crypt_r(senha.getSenha(), cifras[e].data(), &myData);
        int ok = strncmp(result, cifras[e].data(), 14) == 0;

        if (ok) {
          printf("t[%*d, %2.f%%] %s = %s\n", (int)ceil(log10(passo)),
                 inicio - 1, (thread_i / (double)maximo) * 100, cifras[e].data(),
                 senha.getSenha());
          fflush(stdout);

          if (falta.count(e) > 0)
            falta.erase(e);
        }
      }
      if ((int)cifras.size() == 0)
        break;
      senha.prox(passo);

      if (((thread_i + 1) % 100000) == 0) {
        fprintf(stderr, "Realizado %2.f%% ou %llu de %llu\n",
                (thread_i / (double)maximo) * 100, thread_i + 1, maximo);
      }

      counter++;
    }
#pragma omp critical
    { i = std::max(i, thread_i); }
  }
  fprintf(stderr, "Quebrou em %llu iterações!!!!\n", i);

  MPI_Finalize();

  return 0;
}
