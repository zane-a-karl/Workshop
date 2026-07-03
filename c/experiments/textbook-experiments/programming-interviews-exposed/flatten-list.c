/**
   ProbLem Start with a standard doubly linked list. Now imagine that in
   addition to the next and previous pointers, each element has a child
   pointer, which may or may not point to a separate doubly linked list.
   These child lists may have one or more children of their own, and so
   on, to produce a multilevel data structure, as shown in the figure.

   head pointer                                              tail pointer
    |                                                          |
    v                                                          v
+-------+     +-------+     +-------+     +-------+     +-------+
|   5   |<--->|  33   |<--->|  17   |<--->|   2   |<--->|   1   |
|   •   |     |   •   |     |   •   |     |   •   |     |   •   |
| XXXXX |     |   •   |     | XXXXX |     | XXXXX |     | XXXXX |
|   •   |     | XXXXX |     | XXXXX |     |   •   |     | XXXXX |
+-------+     +-------+     +-------+     +-------+     +-------+
    |                                          |
    v                                          v
+-------+     +-------+     +-------+     +-------+     +-------+
|   6   |<--->|  25   |<--->|   6   |     |   2   |<--->|   7   |
|   •   |     |   •   |     | XXXXX |     |   •   |     | XXXXX |
| XXXXX |     |   •   |     |   •   |     | XXXXX |     |   •   |
| XXXXX |     |   •   |     |   •   |     | XXXXX |     | XXXXX |
+-------+     +-------+     +-------+     +-------+     +-------+
                  |               |            |
                  v               v            v
              +-------+     +-------+     +-------+     +-------+
              |   8   |     |   9   |     |  12   |<--->|   5   |
              | XXXXX |     | XXXXX |     |   •   |     | XXXXX |
              | XXXXX |     | XXXXX |     | XXXXX |     |   •   |
              | XXXXX |     |   •   |     |   •   |     | XXXXX |
              +-------+     +-------+     +-------+     +-------+
                                 |             |
                                 v             v
                             +-------+     +-------+     +-------+
                             |   7   |     |  21   |<--->|   3   |
                             | XXXXX |     |   •   |     | XXXXX |
                             | XXXXX |     | XXXXX |     |   •   |
                             | XXXXX |     | XXXXX |     | XXXXX |
                             +-------+     +-------+     +-------+

   Flatten the list so that all the nodes appear in a single-level, doubly
   linked list. You are given the head and tail of the first level of the
   list. Each node is a C struct with the following definition:
   typedef struct Node {
       struct Node *next;
       struct Node *prev;
       struct Node *child;
       int value;
   } Node;
*/

/**
   Let's talk about how you're going to do this.
   Because the problem doesn't specify how we should be flattening the list
   the world is our oyster in choosing the final state our flattened list
   will take.
   Let's list some options and discuss their tradeoffs:
     1. We could just append each child row in succession to the end of
     the parent rows.
       - the output using the above would be
       - 5, 33, 17, 2, 1, 6, 25, 6, 2, 7, 8, 9, 12, 5, 7, 21, 3
       - this is like a breadth first option prioritizing the siblings
     2. We could place the children of a parent just behind them
       - the output using the above would be
       - 5, 6, 25, 8, 6, 9, 7, 33, 17, 2, 2, 12, 21, 3, 5, 7, 1
       - this is more of a depth first strategy where children > siblibings
     3. Off the top of my head no other useful strategies come to mind

   Conclusion:
     - considering (1) is just BFS and (2) is just DFS neither is
     inherently easier or results in a "more natural" output ordering.
     - Because of this I will do both just for educational purposes
     - First, though I need to be able to re-create the example list so
     that I can work through and test my flattening solutions
     - This will require insertion and deletion of nodes and freeing mem

   NOTE: YOU FORGOT THAT IT'S A DOUBLY-LINKED LIST DOES THAT SIMPLIFY THINGS
   OR MAKE IT HARDER BECAUSE THERE'S MORE TO KEEP TRACK OF?
     - It has't been obviously helpful.
     - I haven't thought to myself, "wouldn't it be nice if I could iterate
     backward?".
     - I'm just going to continue on as if it's singly linked but with a child
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Dropped prev it's useless
struct node {
    struct node *next;
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

const size_t INIT_CAP = 4;

bool init_ring_queue(struct ring_queue **q, size_t cap) {
    *q = malloc(sizeof(struct ring_queue));
    if ((*q) == NULL) { return false; }
    (*q)->len = 0;
    (*q)->cap = cap;
    (*q)->head = 0;
    (*q)->tail = 0;
    (*q)->data = malloc(cap * sizeof(*((*q)->data)));
    if ((*q)->data == NULL) {
        free(*q);
        *q = NULL;
        return false;
    }
    return true;
}

bool delete_ring_queue(struct ring_queue **q) {
    if (q == NULL) { return false; }
    // Just free the queue not the linked list
    free((*q)->data);
    free((*q));
    *q = NULL;
    return true;
}

bool queue_push(struct ring_queue **q, struct node *node) {
    if (*q == NULL) init_ring_queue(q, INIT_CAP);
    if ((*q)->len >= (*q)->cap) {
        struct ring_queue *new_q = NULL;
        if (!init_ring_queue(&new_q, 2*(*q)->cap)) { return false; }
        size_t i = (*q)->head, j = (*q)->tail;
        while (i != j) {
            if(!queue_push(&new_q, (*q)->data[i])) { return false; }
            i = (i + 1) % (*q)->cap;
        }
        struct ring_queue *tmp = (*q);
        *q = new_q;
        if(!delete_ring_queue(&tmp)) { return false; }
    }
    (*q)->data[(*q)->tail] = node;
    (*q)->tail = ((*q)->tail + 1) % (*q)->cap;
    (*q)->len++;
    return true;
}

bool queue_pop(struct ring_queue **q, struct node **popped) {
    if (*q == NULL) { return false; }
    if ((*q)->data == NULL) {
        return false;
    }
    if ((*q)->cap >= 4 && (*q)->len <= (*q)->cap/4) {
        struct ring_queue *new_q = NULL;
        if (!init_ring_queue(&new_q, (*q)->cap/2)) { return false; }
        size_t i = (*q)->head, j = (*q)->tail;
        while (i != j) {
            if(!queue_push(&new_q, (*q)->data[i])) { return false; }
            i = (i + 1) % (*q)->cap;
        }
        struct ring_queue *tmp = (*q);
        *q = new_q;
        if(!delete_ring_queue(&tmp)) { return false; }
    }
     *popped = (*q)->data[(*q)->head];
    (*q)->head = ((*q)->head + 1) % (*q)->cap;
    (*q)->len--;
    return true;
}

bool create_node(struct node **new, int data) {
    *new = malloc(sizeof(**new));
    if (*new == NULL) { return false; }
    (*new)->next = NULL;
    (*new)->child = NULL;
    (*new)->data = data;
    return true;
}

/** Older sibling dicates the tier at which our tail
    sibling will be added
 */
bool insert_tail_sibling(struct list      *list,
                         struct node     *older,
                         int               data,
                         struct node **inserted) {

    if (list == NULL) { return false; }
    struct node *new = NULL;
    if (!create_node(&new, data)) { return false; }

    struct node *tmp = NULL;
    // Add to empty list
    if (list->head == NULL) {
        list->head = new;
        *inserted = new;
        return true;
    }
    // Add to top tier
    if (older == NULL) {
        tmp = list->head;
    } else {
        // Add to lower tier
        tmp = older;
    }
    while (tmp->next != NULL) { tmp = tmp->next; }
    tmp->next = new;
    *inserted = new;
    return true;
}

bool insert_child(struct list      *list,
                  struct node    *parent,
                  int               data,
                  struct node **inserted) {

    if (list == NULL) { return false; }
    if (list->head == NULL) { return false; }
    if (parent == NULL) { return false; }
    struct node *new = NULL;
    if (!create_node(&new, data)) { return false; }

    if (parent->child == NULL) {
        parent->child = new;
    } else {
        struct node *tmp = parent->child;
        while (tmp->next != NULL) { tmp = tmp->next; }
        tmp->next = new;
    }
    *inserted = new;
    return true;
}

struct header {
    size_t len;
    size_t cap;
};

bool stack_push(struct node ***stack, struct node *data) {
    if (data == NULL) { return false; }
    struct header *h;
    if (*stack == NULL) {
        // create the stack
        h = malloc(sizeof(struct header) + (INIT_CAP * sizeof(**stack)));
        if (h == NULL) { return false; }
        h->len = 0;
        h->cap = INIT_CAP;
        *stack = (void *)(h + 1);
    }
    h = (struct header *)(*stack) - 1;
    if (h->len >= h->cap) {
        h->cap *= 2;
        h = realloc(h, sizeof(struct header) + (h->cap * sizeof(**stack)));
        if (h == NULL) { return false; }
        *stack = (void *)(h + 1);
    }
    (*stack)[h->len] = data;
    h->len++;
    return true;
}

bool stack_pop(struct node ***stack, struct node **popped) {
    if (*stack == NULL) { return false; }
    struct header *h = (struct header *)(*stack) - 1;
    if (h->cap >= 4 && h->len <= h->cap/4) {
        h->cap /= 2;
        h = realloc(h, sizeof(struct header) + (h->cap * sizeof(**stack)));
        if (h == NULL) { return false; }
        *stack = (void *)(h + 1);
    }
    *popped = (*stack)[h->len - 1];
    h->len--;
    return true;
}

bool delete_list(struct list **list) {
    if (list == NULL || *list == NULL) { return false; }

    struct node **stack = NULL;
    if ((*list)->head != NULL) {
        if(!stack_push(&stack, (*list)->head)) { return false; }
    }

    struct node *curr = NULL;
    while(stack != NULL && 0 != ((struct header *)(stack) - 1)->len) {
        if(!stack_pop(&stack, &curr)) { return false; }

        if (curr->next != NULL) {
            if(!stack_push(&stack, curr->next)) { return false; }
        }        
        if (curr->child != NULL) {
            if(!stack_push(&stack, curr->child)) { return false; }
        }

        printf("Deleted %d\n", curr->data);
        free(curr);
        curr = NULL;
    }
    if (stack != NULL) {
        struct header *h = (struct header *)(stack) - 1;
        free(h);
        h = NULL;
    }
    free(*list);
    *list = NULL;
    return true;
}

bool flatten_list_bfs(struct list **list) {
    if (list == NULL || *list == NULL) { return false; }

    struct ring_queue **queue = NULL;
    
}
void flatten_list_dfs() {}

int main() {
    struct list *l = malloc(sizeof(*l));
    l->head = NULL;
    l->tail = NULL;

    struct node *dn = NULL; // dn = dev null
    struct node *inserted = NULL;
    struct node *five = NULL;
    struct node *six = NULL;
    struct node *twenty_five = NULL;
    struct node *nine = NULL;
    if(!insert_tail_sibling(l, NULL, 5, &five)) { return 1; }
    if(!insert_child(l, five, 6, &six)) { return 1; }
    if(!insert_tail_sibling(l, six, 25, &twenty_five)) { return 1; }
    if(!insert_child(l, twenty_five, 8, &dn)) { return 1; }
    if(!insert_tail_sibling(l, twenty_five, 6, &six)) { return 1; }
    if(!insert_child(l, six, 9, &nine)) { return 1; }
    if(!insert_child(l, nine, 7, &dn)) { return 1; }


    struct node *top_two = NULL;
    struct node *bot_two = NULL;    
    struct node *twelve = NULL;
    struct node *twenty_one = NULL;        
    if(!insert_tail_sibling(l, NULL, 33, &dn)) { return 1; }
    if(!insert_tail_sibling(l, NULL, 17, &dn)) { return 1; }
    if(!insert_tail_sibling(l, NULL, 2, &top_two)) { return 1; }
    if(!insert_child(l, top_two, 2, &bot_two)) { return 1; }
    if(!insert_child(l, bot_two, 12, &twelve)) { return 1; }
    if(!insert_child(l, twelve, 21, &twenty_one)) { return 1; }
    if(!insert_tail_sibling(l, twenty_one, 3, &dn)) { return 1; }
    if(!insert_tail_sibling(l, twelve, 5, &dn)) { return 1; }
    if(!insert_tail_sibling(l, bot_two, 7, &dn)) { return 1; }

    if(!insert_tail_sibling(l, NULL, 1, &dn)) { return 1; }

    if(!delete_list(&l)) { return 1; }
    printf("SUCCESSFULLY DELETED LIST\n");

    return 0;
}
