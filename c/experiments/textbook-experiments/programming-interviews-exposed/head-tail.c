#include <_string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

struct ht_node {
    int *data;
    struct ht_node *next;
};

struct ht_llist {
    struct ht_node *head;
    struct ht_node *tail;
    size_t len;
};

bool insert_at(struct ht_llist *list,
               size_t            idx,
               int             *data)
{
    // We can insert at len to append to the tail
    if (idx > list->len) {
        return false;
    }
    // Create new node
    struct ht_node *new = malloc(sizeof(*new));
    if (NULL == new) {
        printf("ERROR: new is NULL\n");
        return false;
    }
    new->data = malloc(sizeof(*data));
    if (NULL == new->data)  {
        printf("ERROR: new->data is NULL\n");
        return false;
    }
    memcpy(new->data, data, sizeof(*data));

    // Handle insert at head case
    struct ht_node *tmp = NULL;
    if (idx == 0) {
        tmp = list->head;
        list->head = new; // overwrite new head
        new->next = tmp;
        if (tmp == NULL) {
            list->tail = new;
        }
        list->len++;
        return true;
    }

    // Move `itr` to (idx-1)-th node
    // The node before the position we want to add at
    size_t ctr = 0;
    struct ht_node *itr = list->head;
    while (itr != NULL) {
        if (ctr == idx-1) {
            // insert the node
            tmp = itr->next; // save node_(idx+1)
            itr->next = new; // add new node at idx
            new->next = tmp; // pt at node_(idx+1)
            if (idx == list->len) {
                // Does tail need to move?
                list->tail = new; // overwrite new tail
                new->next = NULL;
            }
            list->len++;
            return true;
        }
        ctr++;
        itr = itr->next;
    }
    return false;
}

bool delete_val(struct ht_llist *list,
                int             *data)
{
    if (NULL == data) {
        return false;
    }
    if (list == NULL) {
        return false;
    }
    if (list->head == NULL) {
        return false;
    }
    if (list->head->data == NULL) {
        return false;
    }
    // Handle delete_head case
    if (*(list->head->data) == *data) {
        struct ht_node *tmp = list->head;
        list->head = list->head->next;
        if (list->tail == tmp) {
            list->tail = NULL;
        }
        free(tmp->data);
        free(tmp);
        list->len--;
        return true;
    }
    // Handle all other cases
    struct ht_node *prv = list->head;
    struct ht_node *itr = list->head->next;
    while (itr != NULL) {
        if (*(itr->data) == *data) {
            if (list->tail == itr) {
                list->tail = prv;
            }
            prv->next = itr->next;
            free(itr->data);
            free(itr);
            list->len--;
            return true;
        }
        prv = itr;
        itr = itr->next;
    }
    // data wasn't found
    return false;
}

void print_ht_list(struct ht_llist *list) {
    size_t len = 129;
    char *out = malloc(len * sizeof(*out));
    int data = 0;
    size_t cur = 0;

    // snprintf will null terminate our string for us
    struct ht_node *itr;
    for (itr = list->head; itr != NULL; itr = itr->next) {
        data = *((int *)(itr->data));
        cur += snprintf(out + cur, len - cur, "(");
        if (itr == list->head) {
            cur += snprintf(out + cur, len - cur, "head|");
        }
        cur += snprintf(out + cur, len - cur, "%d", data);
        if (itr == list->tail) {
            cur += snprintf(out + cur, len - cur, "|tail");
        }
        cur += snprintf(out + cur, len - cur, ") -> " );
        if (cur > 3*len/4) {
            out = realloc(out, 2*len);
            if (NULL == out) exit(1);
            len *= 2;
        }
    }
    printf("%sNULL\n", out);
    free(out);
}

const int DATA[] = {69, 420, 1337, 80085};
const size_t DATA_LEN = sizeof(DATA)/sizeof(*DATA);

int main() {

    struct ht_llist list = {NULL, NULL, 0};
    printf("NULL\n");
    for (size_t i = 0; i < DATA_LEN; i++) {
        if (false ==
            insert_at(&list, list.len, (int *)(DATA + i))) {
            printf("Error: insert (%d) at %zu\n", DATA[i], list.len);
        }
        print_ht_list(&list);
    }
    printf("\n--------------\n");
    printf("--------------\n");
    printf("--------------\n\n");

    /* print_ht_list(&list); */
    /* for (size_t i = 0; i < DATA_LEN; i++) { */
    /*     if (false == delete_val(&list, (int *)(DATA + list.len - 1))) { */
    /*         printf("Error: delete (%d)\n", DATA[i]); */
    /*     } */
    /*     print_ht_list(&list); */
    /* } */

    // DATA[] = {69, 420, 1337, 80085};
    int val = 420;
    delete_val(&list, &val);
    print_ht_list(&list); // {69, 1337, 80085};
    val = 69;
    delete_val(&list, &val);
    print_ht_list(&list); // {1337, 80085};
    val = 1337;
    delete_val(&list, &val);
    print_ht_list(&list); // {80085};
    val = 80085;
    delete_val(&list, &val);
    print_ht_list(&list); // {};

    return 0;
}
