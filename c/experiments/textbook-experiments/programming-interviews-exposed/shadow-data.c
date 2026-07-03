#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    size_t count;
    size_t capacity;
} Header;

/* int *arr_init(size_t init_capacity) { */
/*     Header *header = */
/*         malloc(sizeof(int) * init_capacity + sizeof(Header)); */
/*     header->count = 0; */
/*     header->capacity = init_capacity; */
/*     // [header][numbers] */
/*     // ^ */
/*     // data */
/*     return (int *)(header + 1); */
/* } */

#define ARR_INIT_CAPACITY 1

#define arr_push(arr, x)                                                                \
    do {                                                                                \
        if ((arr) == NULL) {                                                            \
          Header *header = malloc(sizeof(*(arr)) * ARR_INIT_CAPACITY + sizeof(Header)); \
          header->count = 0;                                                            \
          header->capacity = ARR_INIT_CAPACITY;                                         \
          (arr) = (void *)(header + 1);                                                 \
        }                                                                               \
      Header * header = (Header *)(arr) - 1;                                            \
      if (header->count >= header->capacity) {                                          \
          header->capacity *= 2;                                                        \
          header = realloc(header, sizeof(*(arr)) * header->capacity + sizeof(Header)); \
          (arr) = (void *)(header + 1);                                                 \
      }                                                                                 \
      (arr)[header->count++] = (x);                                                     \
    } while(0)
/* void arr_push(int *arr, int x) { */
/*     // [header][numbers] */
/*     //         ^ */
/*     //         arr */
/*     Header *header = (Header *)(arr) - 1; */
/*     assert(header->count < header->capacity); */
/*     arr[header->count++] = x; */
/* } */

#define arr_len(arr) ((Header *)(arr) - 1)->count
/* size_t arr_len(int *arr) { */
/*     return ((Header *)(arr) - 1)->count; */
/* } */

#define arr_free(arr) ((Header *)(arr) - 1)

int main() {
    size_t init_capacity = 256;
    /* int *numbers = arr_init(init_capacity); */
    float *numbers = NULL;
    arr_push(numbers, 69);
    arr_push(numbers, 420);
    arr_push(numbers, 1337);
    arr_push(numbers, 80085);
    if (1) arr_push(numbers, 111); else arr_push(numbers, 222);
    for (size_t i = 0; i < arr_len(numbers); i++) {
        printf("numbers[%zu] => %f\n", i, numbers[i]);
    }
    arr_free(numbers);
    return 0;
}

/////////////////////

#define STB_DS_IMPLEMENTATION
#include "./stb_ds.h"

typedef struct {
    const char *key;
    size_t value;
} Item;

Item *table = NULL;

int main2() {
    shput(table, "foo", 69);
    shput(table, "bar", 42);
    shput(table, "baz", 1337);
    int idx = shgeti(table, "bar");
    table[idx].value = 80085;
    for (int i = 0; i < shlen(table); i++) {
        printf("%s => %zu\n", table[i].key, table[i].value);
     }
    return 0;
}
