#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
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

matrix_t *matrix_sort(matrix_t *A)
{
    matrix_t * ret = NULL;
    ret = (matrix_t *) matrix_create(A->rows, A->cols);

    memcpy(ret->data, A->data, sizeof(double *) * A->rows);
    memcpy(ret->data[0], A->data[0], sizeof(double) * A->rows * A->cols);

    bubble_sort(ret->data[0], A->rows * A->cols);

    return ret;
}
