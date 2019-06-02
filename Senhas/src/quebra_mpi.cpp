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
  set<string> cifras;
  string cifra;
  for (int i = 0; i < 400; i++) {
    getline(cin, cifra);
    cifras.insert(cifra);
  }

  // map<string, string> quebradas;

  unsigned long long i = 0L;
#pragma omp parallel
  {
    crypt_data myData;
    set<string> myCifras = cifras;
    char *result = myData.crypt_3_buf;
    int inicio = omp_get_thread_num() + 1;
    int passo = omp_get_num_threads();
    // printf("[%d] inicio %d, passo %d, myData %p\n", inicio - 1, inicio,
    // passo,
    //        &myData);
    Senha senha(inicio);
    unsigned long long thread_i, counter = 0;
    for (thread_i = inicio; thread_i < maximo; thread_i += passo) {

      // Sincronizar entre as threads
      if ((counter % 1000) == 0) {
#pragma omp critical
        {
          if (cifras.size() < myCifras.size())
            myCifras = cifras;
        }
      }
      for (auto &e : myCifras) {
        result = crypt_r(senha.getSenha(), e.data(), &myData);
        int ok = strncmp(result, e.data(), 14) == 0;

        if (ok) {
          // printf("(%d) %s == %s\n", 400 - (int)myCifras.size(),
          //        e.data(), senha.getSenha());
          printf("t[%*d, %2.f%%] %s = %s\n", (int)ceil(log10(passo)),
                 inicio - 1, (thread_i / (double)maximo) * 100, e.data(),
                 senha.getSenha());
          fflush(stdout);
          // quebradas[e.data()] = senha.getSenha();

#pragma omp critical
          {
            if (cifras.count(e) > 0)
              cifras.erase(e);
          }
          // break;
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
  printf("Quebrou em %llu iterações!!!!\n", i);

  return 0;
}
