#include "matrix.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

enum Device { CPU, GPU };

double wtime() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec + t.tv_usec / 1000000.0;
}

int main(int argc, char **argv) {
  char oper[256], device[256];
  double start_time, end_time;
  // Inicializar para perder o warning (-Wmaybe-uninitialized)
  start_time = wtime();
  int width;

  if ((argc != 4)) {
    printf("Uso: %s <operacao> <width> <cpu|gpu>\n", argv[0]);
    printf("   operacao: [sum, mult, sort]\n");
    exit(EXIT_FAILURE);
  }

  strncpy(oper, argv[1], 256);
  width = atoi(argv[2]);
  strncpy(device, argv[3], 256);

  if (strcmp(device, "cpu") && strcmp(device, "gpu")) {
    printf("Dispositivo inválido. Use \"cpu\" ou \"gpu\".\n");
    exit(EXIT_FAILURE);
  }

  int dev = CPU;
  if (strcmp(device, "gpu") == 0) {
    dev = GPU;
  }

  // Do something
  matrix_t *a = (matrix_t *)matrix_create(width, width);
  matrix_t *ret = (matrix_t *)matrix_create(width, width);
  matrix_randfill(a);

  if (strcmp(oper, "sum") == 0) {
    matrix_t *b = (matrix_t *)matrix_create(width, width);
    matrix_randfill(b);
    start_time = wtime();
    if (dev == GPU)
      matrix_sum_gpu(a, b, ret);
    else
      matrix_sum(a, b, ret);
  }

  if (strcmp(oper, "mult") == 0) {
    matrix_t *b = (matrix_t *)matrix_create(width, width);
    matrix_randfill(b);
    start_time = wtime();
    if (dev == GPU)
      matrix_multiply_gpu(a, b, ret);
    else
      matrix_multiply(a, b, ret);
  }

  if (strcmp(oper, "sort") == 0) {
    // memcpy(ret->data, a->data, sizeof(double *) * a->rows);
    // memcpy(ret->data[0], a->data[0], sizeof(double) * a->rows * a->cols);
    start_time = wtime();
    // if (dev == GPU)
    //   matrix_sort_gpu(a, ret);
    // else
    matrix_sort(a, a);
  }
  // END Do something

  end_time = wtime();

  // matrix_print(ret);
  printf("R[%*d,%*d] = %0.5f\n", (int)ceil(log10(width)), 0,
         (int)ceil(log10(width)), 0, ret->data[0][0]);
  printf("R[%*d,%*d] = %0.5f\n", (int)ceil(log10(width)), width - 1,
         (int)ceil(log10(width)), width - 1, ret->data[width - 1][width - 1]);

  // printf("%d %d %f\n", width, width, end_time - start_time);
  printf("%f\n", end_time - start_time);
  fflush(stdout);

  cudaDeviceSynchronize();

  return EXIT_SUCCESS;
}
