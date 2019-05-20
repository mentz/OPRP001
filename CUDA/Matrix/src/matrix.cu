#include "matrix.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

matrix_t *matrix_create(int rows, int cols) {
  matrix_t *ret = NULL;
  double *bloco = NULL;
  int i = 0;

  ret = (matrix_t *)malloc(sizeof(matrix_t));
  bloco = (double *)malloc(sizeof(double) * rows * cols);

  ret->data = (double **)malloc(sizeof(double *) * rows);
  ret->cols = cols;
  ret->rows = rows;

  for (i = 0; i < rows; i++) {
    ret->data[i] = &bloco[i * cols];
  }

  return ret;
}

void matrix_destroy(matrix_t *m) {
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

void matrix_randfill(matrix_t *m) {
  int i, j;
  for (i = 0; i < m->rows; i++) {
    for (j = 0; j < m->cols; j++) {
      m->data[i][j] = random();
    }
  }
}

void matrix_fill(matrix_t *m, double val) {
  int i, j;
  for (i = 0; i < m->rows; i++) {
    for (j = 0; j < m->cols; j++) {
      m->data[i][j] = val;
    }
  }
}

matrix_t *matrix_multiply(matrix_t *A, matrix_t *B, matrix_t *ret) {
  // Checar se a multiplicação é possível
  // if (A->cols != B->rows) {
  //   printf("Matrizes de formato incompativel\n");
  //   return NULL;
  // }

  int i, j, k;
  int newRows = A->rows;
  int newCols = B->cols;

  for (i = 0; i < newRows; i++) {
    for (j = 0; j < newCols; j++) {
      register double sum = 0;
      for (k = 0; k < A->rows; k++) {
        sum += A->data[i][k] * B->data[k][j];
      }
      ret->data[i][j] = sum;
    }
  }

  return ret;
}

matrix_t *matrix_sum(matrix_t *A, matrix_t *B, matrix_t *ret) {
  // Checar se a soma é possível
  // if (A->rows != B->rows || A->cols != B->cols) {
  //   printf("Matrizes de formato incompativel\n");
  //   return NULL;
  // }

  int i;
  int newRows = A->rows;
  int newCols = A->cols;

  for (i = 0; i < newCols * newRows; i++) {
    ret->data[0][i] = A->data[0][i] + B->data[0][i];
  }

  return ret;
}

matrix_t *matrix_sort(matrix_t *A, matrix_t *ret) {
  merge_sort(ret->data[0], A->rows * A->cols);

  return ret;
}

void matrix_print(matrix_t *m) {
  int i, j;
  for (i = 0; i < m->rows; i++) {
    for (j = 0; j < m->cols; j++) {
      printf("%.17f ", m->data[i][j]);
    }
    printf("\n");
  }
  fflush(stdout);
}

// Métodos de GPU

__global__ void matrix_multiply_kernel(double *a, double *b, double *ret,
                                       int ladoMatriz) {
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  if (y < ladoMatriz && x < ladoMatriz) {
    double soma = 0.0;
    for (int i = 0; i < ladoMatriz; i++) {
      soma += a[y * ladoMatriz + i] * b[i * ladoMatriz + x];
    }
    ret[y * ladoMatriz + x] = soma;
  }
}

matrix_t *matrix_multiply_gpu(matrix_t *A, matrix_t *B, matrix_t *ret) {
  // Checar se a multiplicação é possível
  // if (A->cols != B->rows) {
  //   printf("Matrizes de formato incompativel\n");
  //   return NULL;
  // }

  int newRows = A->rows;
  int newCols = B->cols;

  int alloc_mat_size = sizeof(double) * newRows * newCols;
  int width = newRows;

  double *d_a, *d_b, *d_ret;
  cudaMalloc((void **)&d_a, alloc_mat_size);
  cudaMalloc((void **)&d_b, alloc_mat_size);
  cudaMalloc((void **)&d_ret, alloc_mat_size);

  cudaMemcpy(d_a, A->data[0], alloc_mat_size, cudaMemcpyHostToDevice);
  cudaMemcpy(d_b, B->data[0], alloc_mat_size, cudaMemcpyHostToDevice);

  dim3 bloco(CUDA_T2, CUDA_T2);
  dim3 grade((int)ceil((double)width / CUDA_T2),
             (int)ceil((double)width / CUDA_T2));

  matrix_multiply_kernel<<<grade, bloco>>>(d_a, d_b, d_ret, newRows);

  cudaMemcpy(ret->data[0], d_ret, alloc_mat_size, cudaMemcpyDeviceToHost);

  return ret;
}

__global__ void matrix_sum_kernel(double *a, double *b, double *ret,
                                  int num_cells) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < num_cells) {
    ret[i] = a[i] + b[i];
  }
}

matrix_t *matrix_sum_gpu(matrix_t *A, matrix_t *B, matrix_t *ret) {
  // Checar se a soma é possível
  // if (A->rows != B->rows || A->cols != B->cols) {
  //   printf("Matrizes de formato incompativel\n");
  //   return NULL;
  // }

  int newRows = A->rows;
  int newCols = A->cols;

  int alloc_mat_size = sizeof(double) * newRows * newCols;
  int num_cells = newRows * newCols;

  double *d_a, *d_b, *d_ret;
  cudaMalloc((void **)&d_a, alloc_mat_size);
  cudaMalloc((void **)&d_b, alloc_mat_size);
  cudaMalloc((void **)&d_ret, alloc_mat_size);

  cudaMemcpy(d_a, A->data[0], alloc_mat_size, cudaMemcpyHostToDevice);
  cudaMemcpy(d_b, B->data[0], alloc_mat_size, cudaMemcpyHostToDevice);

  int grades = (int)ceil((double)num_cells / CUDA_T);

  matrix_sum_kernel<<<grades, CUDA_T>>>(d_a, d_b, d_ret, num_cells);

  cudaMemcpy(ret->data[0], ret, alloc_mat_size, cudaMemcpyDeviceToHost);

  return ret;
}