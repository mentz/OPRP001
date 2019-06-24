#ifndef _OPRP_WORDGEN_
#define _OPRP_WORDGEN_

#include <string.h>

// #define MIN(a, b) ((a < b) ? a : b)
// #define MAX(a, b) ((a > b) ? a : b)
#define BASE64_CRYPT                                                           \
  "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

typedef unsigned long long ull;

static int maxSize = 64;

class Senha {
private:
  char senha[32];
  int vetor[8];

  void avancarN(int n) {
    int overflow, pos = 0;

    // TODO: Desenrolar esse while
    vetor[pos] += n;
    while ((overflow = (vetor[pos] / maxSize)) > 0 && pos < 8) {
      vetor[pos] = vetor[pos] % maxSize;
      senha[pos] = BASE64_CRYPT[vetor[pos]];
      vetor[++pos] += overflow;
    }
    senha[pos] = BASE64_CRYPT[vetor[pos]];

    // senha[0] = BASE64_CRYPT[vetor[0]];
    // senha[1] = BASE64_CRYPT[vetor[1]];
    // senha[2] = BASE64_CRYPT[vetor[2]];
    // senha[3] = BASE64_CRYPT[vetor[3]];
    // senha[4] = BASE64_CRYPT[vetor[4]];
    // senha[5] = BASE64_CRYPT[vetor[5]];
    // senha[6] = BASE64_CRYPT[vetor[6]];
    // senha[7] = BASE64_CRYPT[vetor[7]];
  }

public:
  Senha(int comeco) {
    vetor[0] = 0;
    vetor[1] = -1;
    vetor[2] = -1;
    vetor[3] = -1;
    vetor[4] = -1;
    vetor[5] = -1;
    vetor[6] = -1;
    vetor[7] = -1;
    memset(senha, 0, sizeof(char) * 16);
    senha[0] = BASE64_CRYPT[0];
    avancarN(comeco);
  }

  Senha() {
    vetor[0] = 0;
    vetor[1] = -1;
    vetor[2] = -1;
    vetor[3] = -1;
    vetor[4] = -1;
    vetor[5] = -1;
    vetor[6] = -1;
    vetor[7] = -1;
    memset(senha, 0, sizeof(char) * 16);
    senha[0] = BASE64_CRYPT[0];
  }

  char *getSenha() { return senha; }

  void prox() { avancarN(1); }

  void prox(int n) { avancarN(n); }
};

#endif