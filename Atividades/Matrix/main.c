#include <stdio.h>
#include <stdlib.h>
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
    double start_time, end_time;
    int nrows, ncols;

    if ((argc != 3)) {
        printf("Uso: %s <rows> <cols>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    nrows = atoi(argv[1]);
    ncols = atoi(argv[2]);

    start_time = wtime();

    //Do something
    matrix_t * a = (matrix_t *) matrix_create(nrows, ncols);
    matrix_randfill(a);
    matrix_t * b = (matrix_t *) matrix_create(nrows, ncols);
    matrix_fill(b, 1.0);

    matrix_t *rm = matrix_multiply(a, b);
    matrix_t *rs = matrix_sum(a, b);

    matrix_t *or = matrix_sort(rm);

    // matrix_print(or);
    // END Do something

    end_time = wtime();

    printf("%d %d %f\n", nrows, ncols, end_time - start_time);
    fflush(stdout);

    return EXIT_SUCCESS;
}
