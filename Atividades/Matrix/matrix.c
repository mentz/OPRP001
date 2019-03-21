#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "matrix.h"
#include "utils.h"

matrix_t * matrix_create(int rows, int cols)
{
    matrix_t * ret = NULL;
    double * bloco = NULL;
    int i = 0;

    ret = (matrix_t *) malloc(sizeof(matrix_t));
    bloco = (double *) malloc(sizeof(double) * rows * cols);

    ret->data = malloc(sizeof(double *) * rows);
    ret->cols = cols;
    ret->rows = rows;

    for(i = 0; i < rows; i++) {
        ret->data[i] = &bloco[i * cols];
    }

    return ret;
}

void matrix_destroy(matrix_t *m)
{
    int i;

    free(m->data[0]);
    for (i = 0; i < m->rows; i++) {
        m->data[i] = NULL;
    }

    free(m->data);
    m->data = NULL;

    m->rows = 0;
    m->cols = 0;

    free(m);
    m = NULL;

    return;
}

void matrix_randfill(matrix_t *m)
{
    int i, j;
    for (i = 0; i < m->rows; i++) {
        for (j = 0; j < m->cols; j++) {
            m->data[i][j] = random();
        }
    }
}

void matrix_fill(matrix_t *m, double val)
{
    int i, j;
    for (i = 0; i < m->rows; i++) {
        for (j = 0; j < m->cols; j++) {
            m->data[i][j] = val;
        }
    }
}

void *matrix_multiply_worker(void *args)
{
    matrix_mult_args_t *margs = (matrix_mult_args_t *) args;

    int i, j, k;

    for(i = margs->i0; i < margs->i1; i++) {
        for(j = 0; j < margs->A->cols; j++) {
            double sum = 0;
            for(k = 0; k < margs->A->rows; k++) {
                sum += margs->A->data[i][k] * margs->B->data[k][j];
            }
            margs->C->data[i][j] = sum;
        }
    }

    return NULL;
}

matrix_t *matrix_multiply_threaded(matrix_t *A, matrix_t *B, int num_threads)
{
    // Checar se a multiplicação é possível
    if (A->cols != B->rows) {
        printf("Matrizes de formato incompativel\n");
        return NULL;
    }
    
    int i;
    matrix_t * ret = NULL;
    int newRows = A->rows;
    int newCols = B->cols;
    ret = (matrix_t *) matrix_create(newRows, newCols);

    matrix_mult_args_t *args = NULL;
    pthread_t *threads = NULL;

    args = (matrix_mult_args_t *) malloc(sizeof(matrix_mult_args_t) * num_threads);
    threads = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);

    int partition_size = newRows / num_threads;

    for (i = 0; i < num_threads; i++) {
        args[i].A = A;
        args[i].B = B;
        args[i].C = ret;

        args[i].i0 = partition_size * i;
        args[i].i1 = partition_size * (i+1);
    }
    args[num_threads - 1].i1 = newRows;

    for (i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, matrix_multiply_worker, (void *) &args[i]);
    }

    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return ret;
}

matrix_t *matrix_multiply(matrix_t *A, matrix_t *B)
{
    // Checar se a multiplicação é possível
    if (A->cols != B->rows) {
        printf("Matrizes de formato incompativel\n");
        return NULL;
    }

    int i, j, k;
    matrix_t * ret = NULL;
    int newRows = A->rows;
    int newCols = B->cols;
    ret = (matrix_t *) matrix_create(newRows, newCols);

    for(i = 0; i < newRows; i++) {
        for(j = 0; j < newCols; j++) {
            double sum = 0;
            for(k = 0; k < A->rows; k++) {
                sum += A->data[i][k] * B->data[k][j];
            }
            ret->data[i][j] = sum;
        }
    }

    return ret;
}

void *matrix_sum_worker(void *args) {
    matrix_sum_args_t *margs = (matrix_sum_args_t *) args;
    int i;

    for (i = 0; i <  margs->len; i++) {
        margs->C[i] = margs->A[i] + margs->B[i];
    }

return NULL;
}

matrix_t *matrix_sum_threaded(matrix_t *A, matrix_t *B, int num_threads) {
    // Checar se a soma é possível
    if (A->rows != B->rows || A->cols != B->cols) {
        printf("Matrizes de formato incompativel\n");
        return NULL;
    }

    int i;
    matrix_t * ret = NULL;
    int newRows = A->rows;
    int newCols = A->cols;
    ret = (matrix_t *) matrix_create(newRows, newCols);

    matrix_sum_args_t *args = NULL;
    pthread_t *threads = NULL;

    args = (matrix_sum_args_t *) malloc(sizeof(matrix_sum_args_t) * num_threads);
    threads = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);

    int partition_size = (newRows * newCols) / num_threads;

    for (i = 0; i < num_threads; i++) {
        args[i].A = A->data[0] + i * partition_size;
        args[i].B = B->data[0] + i * partition_size;
        args[i].C = ret->data[0] + i * partition_size;

        args[i].len = partition_size;
    }
    args[num_threads - 1].len += (newRows * newCols) % partition_size;

    for (i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, matrix_sum_worker, (void *) &args[i]);
    }

    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return ret;
}

matrix_t *matrix_sum(matrix_t *A, matrix_t *B)
{
    // Checar se a soma é possível
    if (A->rows != B->rows || A->cols != B->cols) {
        printf("Matrizes de formato incompativel\n");
        return NULL;
    }

    int i, j;
    matrix_t * ret = NULL;
    int newRows = A->rows;
    int newCols = A->cols;
    ret = (matrix_t *) matrix_create(newRows, newCols);

    for (i = 0; i < newCols; i++) {
        for (j = 0; j < newRows; j++) {
            ret->data[i][j] = A->data[i][j] + B->data[i][j];
        }
    }

    return ret;
}

matrix_t *matrix_sort_threaded(matrix_t *A, int num_threads)
{
    matrix_t * ret = NULL;
    ret = (matrix_t *) matrix_create(A->rows, A->cols);

    memcpy(ret->data, A->data, sizeof(double *) * A->rows);
    memcpy(ret->data[0], A->data[0], sizeof(double) * A->rows * A->cols);

    // bubble_sort(ret->data[0], A->rows * A->cols);
    merge_sort_threaded(ret->data[0], A->rows * A->cols, num_threads);

    return ret;
}

matrix_t *matrix_sort(matrix_t *A)
{
    matrix_t * ret = NULL;
    ret = (matrix_t *) matrix_create(A->rows, A->cols);

    memcpy(ret->data, A->data, sizeof(double *) * A->rows);
    memcpy(ret->data[0], A->data[0], sizeof(double) * A->rows * A->cols);

    // bubble_sort(ret->data[0], A->rows * A->cols);
    merge_sort(ret->data[0], A->rows * A->cols);

    return ret;
}

void matrix_print(matrix_t *m)
{
    int i, j;
    for (i = 0; i < m->rows; i++) {
        for (j = 0; j < m->cols; j++) {
            printf("%.17f ", m->data[i][j]);
        }
        printf("\n");
    }
    fflush(stdout);
}
