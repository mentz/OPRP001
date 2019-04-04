#include "utils.h"

// ISSO É O SONHO

void swap(double *a, double *b) {
  double c = *a;
  *a = *b;
  *b = c;
}

void merge_sort_threaded(double *vec, size_t arr_size, int num_threads) {
  partition_task_t task;
  task.vec = vec;
  task.number = 0;
  task.mythreads = num_threads;
  task.start = 0;
  task.end = arr_size;
  partition_worker((void *)&task);
}

void *partition_worker(void *task) {
  partition_task_t *mytask = (partition_task_t *)task;

  partition_task_t tasks[2];
  int middle = (mytask->start + mytask->end) / 2;

  if (mytask->end - mytask->start > 1) {
    if (mytask->mythreads > 1) {
      tasks[0].vec = mytask->vec;
      tasks[0].number = mytask->number * 2 + 1;
      tasks[0].mythreads = mytask->mythreads / 2;
      tasks[0].start = mytask->start;
      tasks[0].end = middle;

      tasks[1].vec = mytask->vec;
      tasks[1].number = mytask->number * 2 + 2;
      tasks[1].mythreads = (mytask->mythreads / 2) + (mytask->mythreads % 2);
      tasks[1].start = middle;
      tasks[1].end = mytask->end;
      omp_set_num_threads(num_threads);
#pragma omp parallel sections
      {
#pragma omp section
        { partition_worker(&tasks[0]); }
#pragma omp section
        { partition_worker(&tasks[1]); }
      }
    } else {
      // fprintf(stderr, "OI3 %d %d\n", mytask->start, mytask->end);
      partition(mytask->vec, mytask->start, middle);
      partition(mytask->vec, middle, mytask->end);
    }
  }

  merge(mytask->vec, mytask->start, middle, mytask->end);

  return NULL;
}

void merge_sort(double *vec, size_t arr_size) { partition(vec, 0, arr_size); }

void partition(double *vec, int start, int end) {
  int middle = (start + end) / 2;

  if (end - start > 1) {
    partition(vec, start, middle);
    partition(vec, middle, end);
  }

  merge(vec, start, middle, end);
}

void merge(double *vec, int start, int middle, int end) {
  double *lv = (double *)malloc(sizeof(double) * (middle - start));
  memcpy(lv, &vec[start], sizeof(double) * (middle - start));
  double *rv = (double *)malloc(sizeof(double) * (end - middle));
  memcpy(rv, &vec[middle], sizeof(double) * (end - middle));
  int l = 0, ll = middle - start;
  int r = 0, rr = end - middle;
  int k = start;

  while (l < ll && r < rr) {
    if (lv[l] < rv[r]) {
      vec[k++] = lv[l++];
    } else {
      vec[k++] = rv[r++];
    }
  }

  while (l < ll) {
    vec[k++] = lv[l++];
  }

  while (r < rr) {
    vec[k++] = rv[r++];
  }

  free(lv);
  free(rv);
}

void bubble_sort(double *vec, size_t arr_size) {
  int i, k;

  for (k = 0; k < (int)arr_size; k++) {
    for (i = 0; i < (int)arr_size - 1; i++) {
      if (vec[i] > vec[i + 1]) {
        double c = vec[i];
        vec[i] = vec[i + 1];
        vec[i + 1] = c;
        // swap(&vec[i], &vec[i+1]);
      }
    }
  }
}
