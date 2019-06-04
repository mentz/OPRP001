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
    printf("Falta argumento: ./%s <comprimento_maximo>\n", argv[0]);
    exit(1);
  }

  // Ler senhas
  set<int> falta;
  vector<string> cifras;
  string cifra;
  for (int i = 0; i < 400; i++) {
    getline(cin, cifra);
    cifras.push_back(cifra);
    falta.insert(i);
  }

  unsigned long long i = 0L;
#pragma omp parallel
  {
    crypt_data myData;
    char *result = myData.crypt_3_buf;
    int inicio = omp_get_thread_num() + 1;
    int passo = omp_get_num_threads();
    Senha senha(inicio);
    unsigned long long thread_i, counter = 0;
    for (thread_i = inicio; thread_i < maximo; thread_i += passo) {
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
