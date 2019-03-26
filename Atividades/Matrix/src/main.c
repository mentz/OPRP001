#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include "matrix.h"

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1000000.0;
}

int main(int argc, char **argv)
{
    char oper[256];
    double start_time, end_time;
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
    if (nthreads < 1) {
        printf("Número inválido de threads.\n");
        exit(EXIT_FAILURE);
    }

    //Do something
    matrix_t * a = (matrix_t *) matrix_create(nrows, ncols);
    matrix_randfill(a);


    if (strcmp(oper, "sum")) {
        matrix_t * b = (matrix_t *) matrix_create(nrows, ncols);
        matrix_randfill(b);
        start_time = wtime();
        matrix_sum_threaded(a, b, nthreads);
    }

    if (strcmp(oper, "mult")) {
        matrix_t * b = (matrix_t *) matrix_create(nrows, ncols);
        matrix_randfill(b);
        start_time = wtime();
        matrix_multiply_threaded(a, b, nthreads);
    }

    if (strcmp(oper, "sort")) {
        start_time = wtime();
        matrix_sort_threaded(a, nthreads);
    }
    // END Do something

    end_time = wtime();

    printf("%d %d %f\n", nrows, ncols, end_time - start_time);
    fflush(stdout);

    return EXIT_SUCCESS;
}