#include <stdint-gcc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
struct Compare { int val; int index; };
#pragma omp declare reduction(minimum : struct Compare : omp_out = omp_in.val < omp_out.val ? omp_in : omp_out)
void selectionSort(uint32_t *arr, int64_t size) {
    struct Compare minPosition;
    for (int i = 0; i < size; i++)
    {
        minPosition.val = arr[i];
        minPosition.index = i;
        for (int j = i + 1; j < size; j++)
            if (minPosition.val > arr[j])
                minPosition.index = j;
        swap(arr, minPosition.index,i);
    }
}

void parallelSelectionSort(uint32_t *arr, int64_t size) {
    struct Compare minPosition;
    for (int i = 0; i < size; i++)
    {
        minPosition.val = arr[i];
        minPosition.index = i;
        #pragma omp parallel for reduction(minimum:minPosition)
        for (int j = i + 1; j < size; j++)
            if (minPosition.val > arr[j])
                minPosition.index = j;
        swap(arr, minPosition.index,i);
    }
}

int32_t main(int32_t argc, char **argv) {
    int64_t elements = ELEM_NUM;
    int64_t cycles = CYCLE_NUM;
    if (argc > 1) {
        elements = strtol(argv[1], NULL, 10);
    }
    if (argc > 2) {
        cycles = strtol(argv[2], NULL, 10);
    }
//    test_sort(cycles, elements, selectionSort, "selectionSort");
    test_sort(cycles, elements, parallelSelectionSort, "parallelSelectionSort");
    return 0;
}