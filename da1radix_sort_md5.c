#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


const int KEY_SIZE = 32;
const int VALUE_SIZE = 64;
const int RADIX = 16;
const int MEM_COOF = 3;


typedef struct {
    char value[65];
    char key[33];
} Data;


int charToDigit(char c)
{
    if (c <= 'f' && c >= 'a') {
        return c - 'a' + 10;
    } else  {
        return c - '0';
    }
}


void DataRadixSort(Data *data, int number_of_elements, int *array_for_sort, Data *extra_data)
{
    Data *tmp_data;
    for (register int j = KEY_SIZE - 1; j >= 0 ; --j) {
        register int i;
        for (i = 0; i < RADIX; ++i) {
            array_for_sort[i] = 0;
        }
        for (i = 0; i < number_of_elements; ++i) {
            ++array_for_sort[charToDigit(data[i].key[j])];
        }
        for (i = 1; i < RADIX; ++i) {
            array_for_sort[i] += array_for_sort[i - 1];
        }
        for (i = number_of_elements - 1; i >= 0; --i) {
            extra_data[--array_for_sort[charToDigit(data[i].key[j])]] = data[i];
        }
        
        tmp_data = data;
        data = extra_data;
        extra_data = tmp_data;
    }
}


int main()
{
    register int n = 0, cur_mem = 0;
    int array_for_sort[RADIX];
    Data *data = (Data *)calloc(1, sizeof(Data));
    while (scanf("%s\t%s", data[n].key, data[n].value) != EOF) {
        if (cur_mem <= n + 1) {
            cur_mem = (n + 1) * MEM_COOF;
            data = (Data *)realloc(data, cur_mem * sizeof(Data));
        }
        getchar();
        n++;
    }
    
    Data *extra_data = (Data *)calloc(n + 1, sizeof(Data));
    DataRadixSort(data, n, array_for_sort, extra_data);
    
    for (register int  j = 0; j < n; ++j) {
        printf("%s\t%s\n", data[j].key, data[j].value);
    }
    free(extra_data);
    free(data);
    return 0;
}
