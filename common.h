#ifndef HPS1_COMMON_H
#define HPS1_COMMON_H
#define ELEM_NUM 500000
#define CYCLE_NUM 10

#include <stdint-gcc.h>


void test_sort(int64_t cycles, int64_t elements, void (*sort)(uint32_t *, int64_t), char *sort_name);
void swap(uint32_t *arr, int64_t index1, int64_t index2);
#endif //HPS1_COMMON_H
