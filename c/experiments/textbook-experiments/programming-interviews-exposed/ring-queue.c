#include <stdbool.h>
#include <stdlib.h>

struct node {
    struct node *next;
    struct node *prev;
    struct node *child;
    int data;
};

struct list {
    struct node *head;
    struct node *tail;
};

struct ring_queue {
    size_t         len; // # stored items
    size_t         cap;
    size_t        head; // next read
    size_t        tail; // next write
    struct node **data;
};

const size_t ARR_INIT_CAP = 1;

bool init_ring_queue(struct ring_queue **qq, size_t cap) {
    *qq = malloc(sizeof(struct ring_queue));
    struct ring_queue *q = *qq;
    if (!q) return false;
    q->len = 0;
    q->cap = cap;
    q->head = 0;
    q->tail = 0;    
    q->data = malloc(cap * sizeof(*(q->data)));
    if (!q->data) return false;    
    return true;
}

bool delete_ring_queue(struct ring_queue **q) {
    if (q == NULL) return false;
    // Just free the queue not the linked list
    free((*q)->data);
    free((*q));
    return true;
}

bool push_back(struct ring_queue **qq, struct node *node) {
    if (*qq == NULL) return false;    
    struct ring_queue *q = *qq;
    if (q == NULL) return false;
    if (q->len >= q->cap) {
        struct ring_queue *new_q = NULL;
        if (!init_ring_queue(&new_q, 2*q->cap)) return false;
        size_t i = q->head, j = q->tail;
        while (i != j) {
            if(!push_back(&new_q, q->data[i])) return false;
            i = (i + 1) % q->cap;
        }
        struct ring_queue *tmp = q;
        q = new_q;
        if(!delete_ring_queue(&tmp)) return false;
    }
    q->data[q->tail] = node;
    q->tail = (q->tail + 1) % q->cap;
    q->len++;
    return true;
}

bool pop_front(struct ring_queue **qq, struct node **popped) {
    if (*qq == NULL) return false;
    struct ring_queue *q = *qq;
    if (q == NULL) return false;    
    if (q->data == NULL) {
        return false;
    }
    if (q->cap >= 4 && q->len <= q->cap/4) {
        struct ring_queue *new_q = NULL;
        if (!init_ring_queue(&new_q, q->cap/2)) return false;
        size_t i = q->head, j = q->tail;
        while (i != j) {
            if(!push_back(&new_q, q->data[i])) return false;
            i = (i + 1) % q->cap;
        }
    }
     *popped = q->data[q->head];
    q->head = (q->head + 1) % q->cap;
    q->len--;
    return true;
}
