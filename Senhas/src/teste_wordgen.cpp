#include "wordgen.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // Obter comprimento máximo
  int comprimento = 0;
  int print = 0;
  ull maximo = 64L;
  if (argc == 3) {
    sscanf(argv[1], "%d", &comprimento);
    comprimento = MIN(8, comprimento);
    for (int i = 1; i < comprimento; i++) {
      maximo++;
      maximo *= (unsigned long long)maxSize;
    }
    sscanf(argv[2], "%d", &print);
  } else {
    printf("Falta argumento: %s <comprimento_maximo:i> <imprimir:b>\n", argv[0]);
    return 1;
  }

#pragma omp parallel
  {
    int start = omp_get_thread_num();
    // Senha s(start + 1);
    int step = omp_get_num_threads();
    if (!print)
      for (ull i = start; i < maximo; i += step) {
        Senha(i).getSenha();
      }
    else
      for (ull i = start; i < maximo; i += step) {
        printf("%s\n", Senha(i).getSenha());
        // s.prox(step);
      }
  }

  // printf("%llu senhas\n", maximo);

  return 0;
}
