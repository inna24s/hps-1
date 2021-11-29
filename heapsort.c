#include <stdint-gcc.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include <omp.h>

int64_t **divide_by_parts(int64_t size, int32_t parts) {
    int64_t index = 0;
    int64_t **arr_data = calloc(parts, sizeof(void *));
    for (int i = 0; i < parts; ++i) {
        arr_data[i] = calloc(2, sizeof(int64_t));
    }
    int64_t part_size = size / parts;
    for (int i = 0; i < parts - 1; ++i) {
        arr_data[i][0] = part_size;
        arr_data[i][1] = index;
        index += part_size;
        size -= part_size;
    }
    arr_data[parts - 1][0] = size;
    arr_data[parts - 1][1] = index;
    return arr_data;
}

uint32_t min_of_arr(const uint32_t *arr, uint32_t size, const uint32_t *indexes, const uint32_t *sizes) {
    uint32_t min = INT32_MAX;
    uint32_t index = 0;
    for (int i = 0; i < size; ++i) {
        if (indexes[i] < sizes[i]) {
            if (arr[i] < min) {
                min = arr[i];
                index = i;
            }
        }
    }
    return index;
}

void merge_parts(uint32_t *dest_array, int32_t parts_count, uint32_t **arr_parts, int64_t **sizes_and_indexes,
                 int64_t total_size) {
    uint32_t next_nums[parts_count];
    uint32_t indexes[parts_count];
    uint32_t sizes[parts_count];
    memset(indexes, 0, parts_count * sizeof(uint32_t));
    for (int i = 0; i < parts_count; ++i) {
        next_nums[i] = arr_parts[i][0];
        sizes[i] = sizes_and_indexes[i][0];
    }
    for (int i = 0; i < total_size; ++i) {
        uint32_t min_num_index = min_of_arr(next_nums, parts_count, indexes, sizes);
        dest_array[i] = arr_parts[min_num_index][indexes[min_num_index]];
        indexes[min_num_index] += 1;

        next_nums[min_num_index] = arr_parts[min_num_index][indexes[min_num_index]];
    }
}

void heapify(uint32_t *arr, int64_t size, int64_t root) {
    int64_t largest = root;
    int64_t left = 2 * root + 1;
    int64_t right = 2 * root + 2;

    if (left < size && arr[left] > arr[largest]) {
        largest = left;
    }

    if (right < size && arr[right] > arr[largest]) {
        largest = right;
    }

    if (largest != root) {
        swap(arr, root, largest);
        heapify(arr, size, largest);
    }
}

void heapsort(uint32_t *arr, int64_t size) {
    for (int64_t i = size / 2 - 1; i >= 0; i--) {
        heapify(arr, size, i);
    }

    for (int64_t i = size - 1; i >= 0; i--) {
        swap(arr, 0, i);
        heapify(arr, i, 0);
    }
}

void parallelHeapsort(uint32_t *arr, int64_t size) {
    uint32_t **arr_parts;
    int32_t parts_count = omp_get_max_threads();
    int64_t **sizes = divide_by_parts(size, parts_count);
    arr_parts = calloc(sizeof(void *), parts_count);
    for (int i = 0; i < parts_count; ++i) {
        arr_parts[i] = calloc(sizeof(uint32_t), sizes[i][0]);
        memcpy(arr_parts[i], arr + sizes[i][1], sizes[i][0] * sizeof(uint32_t));
    }
    for (int i = 0; i < parts_count; ++i) {
#pragma omp task default(none) shared(arr, sizes, i, arr_parts, size)
        for (int64_t j = size / 2 - 1; j >= 0; j--) {
            heapify(arr, size, i);
        }

        for (int64_t j = size - 1; j >= 0; j--) {
            swap(arr, 0, i);
            heapify(arr, i, 0);
        }
    }
#pragma omp taskwait
    merge_parts(arr, parts_count, arr_parts, sizes, size);
    for (int i = 0; i < parts_count; ++i) {
        free(arr_parts[i]);
        free(sizes[i]);
    }
    free(arr_parts);
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
    test_sort(cycles, elements, parallelHeapsort, "parallelHeapsort");
    test_sort(cycles, elements, heapsort, "Heapsort");
    return 0;
}