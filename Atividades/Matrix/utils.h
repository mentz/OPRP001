#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdlib.h>
#include <string.h>


void dMergeSort(double * vec, size_t arr_size);
void partition(double * vec, int start, int end);
void merge(double * vec, int start, int middle, int end);


void swap(double * a, double * b);
void bubble_sort(double * vec, size_t arr_size);

#endif
