#include <omp.h>
#include <malloc.h>
#include <string.h>
#include <bits/types/clock_t.h>
#include <bits/types/struct_timeval.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include "common.h"

void swap(uint32_t *arr, int64_t index1, int64_t index2) {
    uint32_t tmp = arr[index1];
    arr[index1] = arr[index2];
    arr[index2] = tmp;
}

uint32_t *generate_rand_numbers(uint32_t count) {
    uint32_t *result = calloc(count, sizeof(uint32_t));
    for (int i = 0; i < count; ++i) {
        result[i] = rand();
    }
    return result;
}

void test_sort(int64_t cycles, int64_t elements, void (*sort)(uint32_t *, int64_t), char *sort_name) {
#pragma omp single
    {
        printf("Preparing %ld elements for %s\n", elements, sort_name);
        //массив рандомных чисел
        uint32_t *src_arr = generate_rand_numbers(elements);
        //выделяем память на отсортированный массив
        uint32_t *arr_to_sort = calloc(elements, sizeof(uint32_t));

        double min = DBL_MAX;
        double max = DBL_MIN;
        long double real_time_sum = 0;
        printf("Starting %ld sorts of %ld elements. Sort type: %s\n", cycles, elements, sort_name);

        struct timeval tv1, tv2;

        double real_time;
        for (int i = 0; i < cycles; ++i) {
            memcpy(arr_to_sort, src_arr, sizeof(uint32_t) * elements);

            gettimeofday(&tv1, NULL);
            sort(arr_to_sort, elements);


            gettimeofday(&tv2, NULL);

            real_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
            if (real_time > max) {
                max = real_time;
            }
            if (real_time < min) {
                min = real_time;
            }
            printf("%d sort is done for %.2fs\n", i+1,real_time);

            real_time_sum += real_time;
        }


        free(src_arr);
        free(arr_to_sort);

        puts("___________________________");
        printf("Mean time is: %Lf seconds\n", real_time_sum / cycles);
        printf("Time varies between:\n"
               "    min: %f seconds\n"
               "    max: %f seconds\n", min, max);
        printf("%ld sorts of %ld elements took %Lfs of real time\n", cycles, elements,
               real_time_sum);
    }
}