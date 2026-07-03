#include <_string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

struct ht_node {
    void *data;
    size_t sz;
    struct ht_node *next;
};

struct ht_llist {
    struct ht_node *head;
    struct ht_node *tail;
    size_t len;
};

bool insert_after(struct ht_node *subj,
                  void           *data,
                  size_t            sz,
                  struct ht_llist *list)
{
    struct ht_node *new = malloc(sizeof(*new));
    if (NULL == new) return false;
    new->data = malloc(sz);
    if (NULL == new->data) return false;
    memcpy(new->data, data, sz);
    new->sz = sz;

    struct ht_node *tmp;
    // Does head need to move?
    if (NULL == subj) {
        tmp = list->head; // save old head
        list->head = new; // overwrite new head
        new->next = tmp;  // old head is 2nd
    } else {
        // Always change subj's next if !NULL
        tmp = subj->next;
        subj->next = new;
        new->next = tmp;
    }
    // Does tail need to move?
    if (list->tail == subj) {
        list->tail = new; // overwrite new tail
    }

    list->len++;
    return true;
}

bool delete(struct ht_node  *subj,
            struct ht_llist *list,
            struct ht_node  **out)
{
    if (subj == NULL) {
        return false;
    }
    struct ht_node *prv = list->head;
    for (struct ht_node *itr = list->head; itr != NULL; itr = itr->next) {
        if (itr == subj) {
            *out = subj;
            if (list->head == subj) {
                list->head = itr->next;
            }
            if (list->tail == subj) {
                list->tail = prv;
            }
            prv->next = subj->next;
            list->len--;
            return true;
        }
        prv = itr;
    }
    // subj wasn't found
    return false;
}

void print_ht_list(struct ht_llist *list) {
    size_t len = 129;
    char *out = malloc(len * sizeof(*out));
    int data = 0;
    size_t cur = 0;

    // snprintf will null terminate our string for us
    for (struct ht_node *itr = list->head; itr != NULL; itr = itr->next) {
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
            insert_after(list.tail,
                         (void *)(DATA + i),
                         sizeof(*DATA),
                         &list)) {
            printf("Error: insert_after (%d)", DATA[i]);
        }
        print_ht_list(&list);
    }
    printf("--------------\n");
    print_ht_list(&list);
    struct ht_node *out = malloc(sizeof(*out));
    for (size_t i = 0; i < DATA_LEN; i++) {
        if (false == delete(list.tail, &list, &out)) {
            printf("Error: delete (%d)", DATA[i]);
        }
        free(out->data);
        free(out);
        out = NULL;
        print_ht_list(&list);
    }
    return 0;
}
