#include <crypt.h>
#include <iostream>
#include <map>
#include <math.h>
#include <omp.h>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

static int maxSize = 65;
static const char alfa[66] =
    // "# ./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    // " ./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    // Acontece que não tem espaços mesmo, afinal.
    "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

class Senha {
private:
  char senha[32];
  int vetor[8];

  void avancarN(int n) {
    int overflow, pos = 0;
    vetor[pos] += n;
    while ((overflow = (vetor[pos] / maxSize)) > 0 && pos < 8) {
      vetor[pos] %= maxSize;
      // if (vetor[pos] == 0)
      //   vetor[pos] += 1;
      vetor[++pos] += overflow;
    }
  }

public:
  Senha(int comeco) {
    vetor[0] = -1;
    vetor[1] = -1;
    vetor[2] = -1;
    vetor[3] = -1;
    vetor[4] = -1;
    vetor[5] = -1;
    vetor[6] = -1;
    vetor[7] = -1;
    avancarN(comeco);
  }

  Senha() {
    vetor[0] = -1;
    vetor[1] = -1;
    vetor[2] = -1;
    vetor[3] = -1;
    vetor[4] = -1;
    vetor[5] = -1;
    vetor[6] = -1;
    vetor[7] = -1;
  }

  char *getSenha() {
    int i = 0;
    for (; i < 8 && vetor[i] > -1; i++) {
      senha[i] = alfa[vetor[i]];
    }
    senha[i] = '\0';
    return senha;
  }

  void prox() { avancarN(1); }

  void prox(int n) { avancarN(n); }
};

int main(int argc, char *argv[]) {
  // Obter comprimento máximo
  int comprimento = 0;
  unsigned long long maximo = 1L;
  if (argc == 2) {
    sscanf(argv[1], "%d", &comprimento);
    comprimento = std::min(8, comprimento);
    for (int i = 0; i < comprimento; i++) {
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

  map<string, string> quebradas;

  unsigned long long i = 0L;
#pragma omp parallel
  {
    crypt_data myData;
    set<string> myCifras = cifras;
    char *result = myData.crypt_3_buf;
    int eu = omp_get_thread_num();
    int passo = omp_get_num_threads();
    // printf("[%d] inicio %d, passo %d, myData %p\n", eu - 1, eu, passo,
    // &myData);
    Senha senha(eu + 1);
    unsigned long long thread_i = eu;
    for (; thread_i < maximo; thread_i += passo) {
      if ((rand() % 100) < 2) {
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
          printf("t[%*d, %llu] %s = %s\n", (int)ceil(log10(passo)), eu,
                 thread_i, e.data(), senha.getSenha());
          fflush(stdout);
          quebradas[e.data()] = senha.getSenha();
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
        fprintf(stderr, "%llu\n", thread_i + 1);
      }
    }
#pragma omp critical
    { i = std::max(i, thread_i); }
  }
  printf("Quebrou em %llu iterações!!!!\n", i);

  return 0;
}
