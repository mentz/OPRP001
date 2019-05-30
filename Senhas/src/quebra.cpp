#include <crypt.h>
#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

class Senha {
private:
  int maxSize;
  char alfa[128];
  char senha[32];
  unsigned char vetor[8];

  void avancarN(int n) {
    int overflow, pos = 0;
    vetor[pos] += n;
    while ((overflow = (vetor[pos] / maxSize)) > 0 && pos < 8) {
      vetor[pos] %= maxSize;
      if (vetor[pos] == 0)
        vetor[pos] += 1;
      vetor[++pos] += overflow;
    }
  }

public:
  Senha(int comeco) {
    maxSize = 66;
    strncpy(alfa, "# ./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", sizeof(char) * (maxSize + 2));
    vetor[0] = 0;
    vetor[1] = 0;
    vetor[2] = 0;
    vetor[3] = 0;
    vetor[4] = 0;
    vetor[5] = 0;
    vetor[6] = 0;
    vetor[7] = 0;
    avancarN(comeco);
  }

  char *getSenha() {
    int i = 0;
    for (; i < 8 && vetor[i]; i++) {
      senha[i] = alfa[vetor[i]];
    }
    senha[i] = '\0';
    return senha;
  }

  void prox() { avancarN(1); }

  void prox(int n) { this->avancarN(n); }
};

int main(int argc, char *argv[]) {
  // Ler senhas
  char cifra[32] = "Ha0Vy6cN2SuiM";
  vector<string> cifras(400);
  for (int i = 0; i < 400; i++) {
    getline(cin, cifras[i]);
  }

  char *result = new char[32];
  Senha senha(1);
  unsigned long long maximo = 66L * 66L * 66L; // * 66L * 66L * 66L * 66L * 66L;
  unsigned long long i = 0L;
  for (; i < maximo; i++) {
    // printf("Senha %6d: %s\n", i, senha.getSenha().data());
    for (int j = 0; j < (int)cifras.size(); j++) {
      result = crypt(senha.getSenha(), cifras[j].data());
      int ok = strncmp(result, cifras[j].data(), 14) == 0;

      if (ok) {
        printf("(%d) %s == %s\n", 400 - (int)cifras.size(), cifras[j].data(),
               senha.getSenha());
        cifras.erase(cifras.begin() + j);
        j--;
        break;
      }
    }
    if ((int)cifras.size() == 0)
      break;
    senha.prox();
    if ((i % 100000) == 0)
      printf("%llu\n", i);
  }
  printf("Quebrou em %llu iterações!!!!\n", i);

  return 0;
}