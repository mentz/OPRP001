#include "utils.h"

// ISSO É O SONHO

void swap(double * a, double * b) {
    double c = *a; *a = *b; *b = c;
}

void dMergeSort(double * vec, size_t arr_size)
{
    partition(vec, 0, arr_size - 1);
}

void partition(double * vec, int start, int end)
{
    int middle = (start + end) / 2;

    if (end - start > 1) {
        partition(vec, start, middle);
        partition(vec, middle + 1, end);
    }

    merge(vec, start, middle, end);
}

void merge(double * vec, int start, int middle, int end)
{
    double *lv = malloc(sizeof(double) * (middle - start + 1));
    memcpy(lv, &vec[start], middle-start);
    double *rv = malloc(sizeof(double) * (end - middle + 1));
    memcpy(rv, &vec[middle], end-middle);
    int l = start;
    int r = middle;
    int k = start;

    while (l <= middle && r <= end) {
        if (lv[l] < rv[r]) {
            vec[k++] = lv[l++];
        } else {
            vec[k++] = rv[r++];
        }
    }

    while (l <= middle) {
        vec[k++] = lv[l++];
    }

    while (l <= middle) {
        vec[k++] = lv[l++];
    }
}

void bubble_sort(double * vec, size_t arr_size) {
    int i, k;

    for (k = 0; k < arr_size; k++) {
        for (i = 0; i < arr_size - 1; i++) {
            if (vec[i] > vec[i+1]) {
                double c = vec[i];
                vec[i] = vec[i+1];
                vec[i+1] = c;
                // swap(&vec[i], &vec[i+1]);
            }
        }
    }
}
