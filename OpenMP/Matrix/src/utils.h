#ifndef __UTILS_H__
#define __UTILS_H__

#include <math.h>
#include <omp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(double *a, double *b);

typedef struct {
  double *vec;
  int number, mythreads, start, end;
} partition_task_t;

void merge_sort_threaded(double *vec, size_t arr_size, int num_threads);
void *partition_worker(void *partition_task);
void partition_threaded(double *vec, int threads, int start, int end);

void merge_sort(double *vec, size_t arr_size);
void partition(double *vec, int start, int end);
void merge(double *vec, int start, int middle, int end);

void bubble_sort(double *vec, size_t arr_size);

#endif
