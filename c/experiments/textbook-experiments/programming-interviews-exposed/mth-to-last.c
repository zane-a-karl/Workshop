#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/**
   Given a singly linked list, devise a time- and space-efficient algorithm
  to find the mth-to-last element of the list. Implement your algorithm,
  taking care to handle relevant error conditions. Define mth to last such
  that when m = 0 the last element of the list is returned.
 */

/**
   Thoughts:
    - A naive approach is to simply loop through the list twice
      - first to get the length
      - second to get the mth-to-last element
      - The time complexity is O(n) and space is constant
    - But can we do better?
      - Well, because this is singly linked we can't iterate backward
      - And, check with interviewer, but I will assume we don't have
      the length stored in the list struct ahead of time
      - At the moment I think the best we can do is reduce the constant in
      the time complexity
      - Two pointer approach
        - Create two pointers pointing at the list's head node
        - Iterate through the list only moving the first pointer
        - Once the first pointer has reached the mth element (0-indexed)
        we keep iterating until itr->next == NULL but now we move the
        second pointer along at the same rate as the first.
        - When the first pointer is done the second pointer will be at the
        mth-to-last element.
 */
struct ll_node {
    int data;
    struct ll_node *next;
};

struct linked_list {
    struct ll_node *head;
    struct ll_node *mth_to_last;
};

bool insert_at(struct linked_list *list,
               size_t               idx,
               int                 data) {
    if (NULL == list) {
        return false;
    }
    // Create new node
    struct ll_node *new = malloc(sizeof(*new));
    if (!new) {
        return false;
    }
    new->data = data;
    new->next = NULL;

    struct ll_node *tmp = NULL;
    if (idx == 0) {
        // Handles head == NULL case as well
        tmp = list->head;
        list->head = new;
        new->next = tmp;
        return true;
    }
    struct ll_node *itr = list->head;
    size_t i = 0;
    while (itr != NULL) {
        if (i == idx-1) {
            tmp = itr->next;
            itr->next = new;
            new->next = tmp;
            return true;
        }
        i++;
        itr = itr->next;
    }
    return false;
}

bool mth_to_last(struct linked_list *list,
                 size_t                 m) {

    if (list == NULL) {
        return false;
    }
    if (list->head == NULL) {
        return false;
    }
    struct ll_node *itr1 = list->head;
    struct ll_node *itr2 = list->head;
    size_t nodes_bn = 0;
    while (itr1->next != NULL) {
        if (nodes_bn == m) {
            itr2 = itr2->next;
        } else {
            nodes_bn++;
        }
        itr1 = itr1->next;
    }
    if (nodes_bn < m) {
        list->mth_to_last = NULL;        
        return false;
    }
    list->mth_to_last = itr2;
    return true;
}

bool delete_data(struct linked_list *list,
                 int                 data) {
    if (NULL == list) {
        return false;
    }
    struct ll_node *tmp = NULL;
    if (list->head->data == data) {
        tmp = list->head;
        list->head = list->head->next;
        free(tmp);
        return true;
    }
    struct ll_node *itr = list->head;
    struct ll_node *prv = NULL;
    while (itr != NULL) {
        if (itr->data == data) {
            tmp = itr;
            // prv->next guaranteed to exist because
            //  head case is handled
            prv->next = itr->next;
            free(tmp);
            return true;
        }
        prv = itr;
        itr = itr->next;        
    }
    return false;
}

void print_linked_list(struct linked_list *list) {
    struct ll_node *itr = list->head;
    while (itr != NULL) {
        printf("(%d) -> ", itr->data);
        itr = itr->next;
    }
    printf("NULL\n");
}

const int DATA[] = {420, 69, 1337, 80085, 8414};
const int DATA_LEN = sizeof(DATA)/sizeof(*DATA);
int main() {
    struct linked_list list = { NULL, NULL };
    for (size_t i = 0; i < DATA_LEN; i++) {
        if (!insert_at(&list, i, DATA[i])) {
            printf("ERROR: insert_at %zu, %d\n", i, DATA[i]);
        }
        print_linked_list(&list);
    }
    
    if (!mth_to_last(&list, 0)) return 1;
    printf("0nd-to-last: %d\n", list.mth_to_last->data);
    // delete can mutate mth-to-last beware!
    if (!delete_data(&list, 8414)) return 1;
    print_linked_list(&list);
    if (!mth_to_last(&list, 0)) return 1;
    printf("0nd-to-last: %d\n", list.mth_to_last->data);
    if (!delete_data(&list, 420)) return 1;
    print_linked_list(&list);
    if (!mth_to_last(&list, 0)) return 1;
    printf("0nd-to-last: %d\n", list.mth_to_last->data);

    struct ll_node *tmp = NULL;
    for (struct ll_node *itr = list.head; itr != NULL;) {
        tmp = itr->next;
        if (!delete_data(&list, itr->data)) return 1;
        itr = tmp;
    }
    return 0;
}
