#include "utils.h"

/*
// ISSO É O SONHO
void dMergeSort(double * vec, size_t arr_size)
{
    partition(vec, 0, arr_size);
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

double merge(double * vec, int start, int middle, int end)
{
    int left = start;
    int right = middle + 1;
    int l = start, ll = middle;
    int r = middle + 1, rr = end - 1;
    int k = end - start - 1;

    while (k--) {
        if (l >= ll || r >= rr) break;

        if (vec[l] < vec[r]) {

        }
    }
}
*/

void swap(double * a, double * b) {
    double c = *a; *a = *b; *b = c;
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
