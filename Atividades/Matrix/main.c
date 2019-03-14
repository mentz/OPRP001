#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "matrix.h"

void * thread_job_soma

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1000000.0;
}

int main(int argc, char **argv)
{
    double start_time, end_time;
    int nrows, ncols, nthreads;

    if ((argc != 4)) {
        printf("Uso: %s <rows> <cols> <num_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    nrows = atoi(argv[1]);
    ncols = atoi(argv[2]);
    nthreads = atoi(argv[3]);

    start_time = wtime();

    //Do something
    matrix_t * a = (matrix_t *) matrix_create(nrows, ncols);
    matrix_randfill(a);
    matrix_t * b = (matrix_t *) matrix_create(nrows, ncols);
    matrix_fill(b, 1.0);

    // matrix_t *rm = matrix_multiply(a, b);
    // matrix_t *rs = matrix_sum(a, b);
    matrix_t *rs = matrix_sum_threaded(a, b, num_threads);

    // matrix_t *or = matrix_sort(rm);

    // matrix_print(or);
    // END Do something

    end_time = wtime();

    printf("%d %d %f\n", nrows, ncols, end_time - start_time);
    fflush(stdout);

    return EXIT_SUCCESS;
}


void *