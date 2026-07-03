#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

struct ll_stack_node {
    void *data;
    size_t data_n;
    struct ll_stack_node *down;
};

bool push(struct ll_stack_node **top,
          void                 *data,
          size_t              data_n)
{
    // create new node
    struct ll_stack_node *new = malloc(sizeof(*new));
    if (new == NULL) {
        printf("Failed to malloc new node\n");
        return false;
    }
    new->data = malloc(data_n);
    memcpy(new->data, data, data_n);
    new->data_n = data_n;

    // set the new top
    if (*top != NULL) {
        new->down = *top;
    }
    *top = new;
    return true;
}

bool pop(struct ll_stack_node **top,
          void         **popped_data)
{
    if (*top == NULL) {
        return false;
    }
    // wipe top's contents
    struct ll_stack_node *shallow_copy_top = *top;
    memcpy(*popped_data, (*top)->data, (*top)->data_n);
    free((*top)->data);
    *top = (*top)->down;
    free(shallow_copy_top);

    return true;
}

void create_ll_stack(struct ll_stack_node **stack)
{
  *stack = NULL;
}

bool delete_ll_stack(struct ll_stack_node **top)
{
    void *popped_data = malloc((*top)->data_n);
    if (popped_data == NULL) {
        printf("Failed to malloc top's local data\n");
        return false;
    }
    while (*top != NULL) {
        if (!pop(top, &popped_data)) {
            return false;
        }
    }
    free(popped_data);
    return false;
}

void print_ll_stack(struct ll_stack_node *top) {
    while (top != NULL) {
        printf("%s -> ", *((char **)(top->data)));
        top = top->down;
    }
    printf("NULL\n");
}

/*******************************************************

 *******************************************************/

char *data[] = {"aaa", "bbb", "ccc", "ddd", "eee"};

int main()
{
    struct ll_stack_node *s;
    create_ll_stack(&s);
    for (size_t i = 0; i < sizeof(data)/sizeof(*data); i++) {
        if (!push(&s, (void *)(&data[i]), sizeof(data[i]))) {
            printf("Push failure\n");
            exit(1);
        }
        /* printf("Pushed %s\n", data[i]); */
        print_ll_stack(s);
    }
    printf("\n--------------------\n\n");
    void *popped_data = malloc(s->data_n);
    if (popped_data == NULL) {
        printf("Failed to malloc stack's local data\n");
        exit(1);
    }    
    for (size_t i = 0; i < sizeof(data)/sizeof(*data); i++) {
        if (!pop(&s, &popped_data)) {
            printf("Pop failure\n");
            exit(1);
        }
        /* printf("Popped %s\n", *((char **)popped_data)); */
        print_ll_stack(s);
    }
    free(popped_data);    
    return 0;
}
