#include "utils.h"

// ISSO É O SONHO

void swap(double *a, double *b) {
  double c = *a;
  *a = *b;
  *b = c;
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
