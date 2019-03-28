#include "utils.h"

// ISSO É O SONHO

void swap(double * a, double * b) {
    double c = *a; *a = *b; *b = c;
}

void merge_sort_threaded(double * vec, size_t arr_size, int num_threads)
{
    partition_task_t task;
    task.vec = vec;
    task.number = 0;
    task.mythreads = num_threads;
    task.start = 0;
    task.end = arr_size;
    partition_worker((void *) &task);
}

void *partition_worker(void *task) // (double *vec, int mythreads, int start, int end)
{
    partition_task_t *mytask =  (partition_task_t *) task;

    pthread_t threads[2];
    partition_task_t tasks[2];
    int middle = (mytask->start + mytask->end) / 2;
    mytask->mythreads--;

    if (mytask->end - mytask->start > 1) {
        if (mytask->mythreads > 0) {
            tasks[0].vec = mytask->vec;
            tasks[0].number = mytask->number * 2 + 1;
            tasks[0].mythreads = mytask->mythreads/2;
            tasks[0].start = mytask->start;
            tasks[0].end = middle;
            pthread_create(&threads[0], NULL, partition_worker, &tasks[0]);
        }
        else
            partition(mytask->vec, mytask->start, middle);

        if (mytask->mythreads > 1) {
            tasks[1].vec = mytask->vec;
            tasks[1].number = mytask->number * 2 + 2;
            tasks[1].mythreads = (mytask->mythreads/2) + (mytask->mythreads%2);
            tasks[1].start = middle;
            tasks[1].end = mytask->end;
            pthread_create(&threads[1], NULL, partition_worker, &tasks[1]);
        }
        else
            partition(mytask->vec, middle, mytask->end);
    }

    // join as duas (ou uma) filhas
    if (mytask->mythreads) {
        pthread_join(threads[0], NULL);
    }
    if (mytask->mythreads > 1) {
        pthread_join(threads[1], NULL);
    }

    merge(mytask->vec, mytask->start, middle, mytask->end);

    return NULL;
}

void merge_sort(double * vec, size_t arr_size)
{
    partition(vec, 0, arr_size);
}

void partition(double * vec, int start, int end)
{
    int middle = (start + end) / 2;

    if (end - start > 1) {
        partition(vec, start, middle);
        partition(vec, middle, end);
    }

    merge(vec, start, middle, end);
}

void merge(double *vec, int start, int middle, int end)
{
    double *lv = malloc(sizeof(double) * (middle - start));
    memcpy(lv, &vec[start], sizeof(double) * (middle - start));
    double *rv = malloc(sizeof(double) * (end - middle));
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
