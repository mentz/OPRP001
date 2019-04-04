#include "matrix.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

double wtime() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec + t.tv_usec / 1000000.0;
}

int main(int argc, char **argv) {
  char oper[256];
  double start_time, end_time;
  // Inicializar para perder o warning (-Wmaybe-uninitialized)
  start_time = wtime();
  int nrows, ncols, nthreads;

  if ((argc != 5)) {
    printf("Uso: %s <operacao> <rows> <cols> <num_threads>\n", argv[0]);
    printf("   operacao: [sum, mult, sort]\n");
    exit(EXIT_FAILURE);
  }

  strcpy(oper, argv[1]);
  nrows = atoi(argv[2]);
  ncols = atoi(argv[3]);
  nthreads = atoi(argv[4]);
  // fprintf(stderr, "%d threads\n", nthreads);
  if (nthreads < 1) {
    printf("Número inválido de threads.\n");
    exit(EXIT_FAILURE);
  }

  // Do something
  matrix_t *a = (matrix_t *)matrix_create(nrows, ncols);
  matrix_randfill(a);

  if (strcmp(oper, "sum") == 0) {
    matrix_t *b = (matrix_t *)matrix_create(nrows, ncols);
    matrix_t *ret = (matrix_t *)matrix_create(nrows, ncols);
    matrix_randfill(b);
    start_time = wtime();
    if (nthreads > 1)
      matrix_sum_threaded(a, b, ret, nthreads);
    else
      matrix_sum(a, b, ret);
  }

  if (strcmp(oper, "mult") == 0) {
    matrix_t *b = (matrix_t *)matrix_create(nrows, ncols);
    matrix_t *ret = (matrix_t *)matrix_create(nrows, ncols);
    matrix_randfill(b);
    start_time = wtime();
    if (nthreads > 1)
      matrix_multiply_threaded(a, b, ret, nthreads);
    else
      matrix_multiply(a, b, ret);
  }

  if (strcmp(oper, "sort") == 0) {
    matrix_t *ret = (matrix_t *)matrix_create(nrows, ncols);
    memcpy(ret->data, a->data, sizeof(double *) * a->rows);
    memcpy(ret->data[0], a->data[0], sizeof(double) * a->rows * a->cols);
    start_time = wtime();
    if (nthreads > 1)
      matrix_sort_threaded(a, ret, nthreads);
    else
      matrix_sort(a, ret);
  }
  // END Do something

  end_time = wtime();

  // printf("%d %d %f\n", nrows, ncols, end_time - start_time);
  printf("%f\n", end_time - start_time);
  fflush(stdout);

  return EXIT_SUCCESS;
}