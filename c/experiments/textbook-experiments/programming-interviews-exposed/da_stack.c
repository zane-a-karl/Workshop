#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
/* #include <stdint.h> */

struct da_stack {
    void *da;
    size_t capacity;
    size_t length;
    size_t data_n;
};

bool push(struct da_stack *s,
          void         *data,
          size_t      data_n)
{
    // Check if datum byte length is consistent
    if (data_n != s->data_n) {
        printf("Attempted to push inconsistent data size\n");
        return false;
    }
    // Check if we're full
    if (s->length == s->capacity) {
        void *tmp;
        tmp = realloc(s->da, 2 * s->capacity * data_n);
        if (NULL == tmp) {
            return false;
        }
        s->da = tmp;
        s->capacity *= 2;
    }
    // "push" the new datum
    // Cast to (char *) for portable byte arithmetic
    memcpy((char *)s->da + (data_n * s->length), data, data_n);
    s->length++;
    return true;
}

bool pop(struct da_stack *s,
         void        **data)
{
    // Check if there's nothing to pop
    if (s->length == 0) {
        printf("There's nothing to pop\n");
        return false;
    }
    // Check if we're a quarter full and reduce by half
    if (s->length == s->capacity / 4) {
        void *tmp;
        tmp = realloc(s->da, (s->capacity / 2) * s->data_n);
        if (NULL == tmp) {
            return false;
        }
        s->da = tmp;
        s->capacity /= 2;
    }
    // Save the datum
    memcpy(*data,(char *)s->da + (s->data_n * (s->length - 1)), s->data_n);
    // Not strictly necessary but it feels right to zero out after popping
    memset((char *)s->da + (s->data_n * (s->length - 1)), 0, s->data_n);
    s->length--;
    return true;
}

void create_da_stack(struct da_stack **s,
                     size_t       data_n)
{
    const size_t LEN = 1;
    (*s) = malloc(sizeof(**s));
    (*s)->da = malloc(data_n);
    (*s)->capacity = LEN;
    (*s)->length = 0;
    (*s)->data_n = data_n;
}

bool delete_da_stack(struct da_stack **s)
{
    free((*s)->da);
    free(*s);
    return true;
}

void print_da_stack(struct da_stack *s)
{
    printf("NULL");
    for (size_t i = 0; i < s->length; i++) {
        printf(" <- %d", *((int *)((char *)s->da + i*s->data_n)));
    }
    printf("\n");
}

int data[] = {1, 2, 3, 4, 5};

int main() {
    struct da_stack *stack = NULL;
    create_da_stack(&stack, sizeof(*data));
    for (size_t i = 0; i < sizeof(data)/sizeof(*data); i++) {
        if (!push(stack, (void *)(&data[i]), sizeof(data[i]))) {
            printf("Push error\n");
            exit(1);
        }
        print_da_stack(stack);
    }
    printf("\n--------------------\n\n");
    void *popped_data = malloc(stack->data_n);
    for (size_t i = 0; i < sizeof(data)/sizeof(*data); i++) {
        if (!pop(stack, &popped_data)) {
            printf("Pop error\n");
            exit(1);
        }
        print_da_stack(stack);        
    }
    free(popped_data);
    return 0;
}
