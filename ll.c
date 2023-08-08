#include "ll.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t seeks = 0;
uint64_t links = 0;

struct LinkedList {
    uint32_t length;
    Node *head; // Head sentinel node
    Node *tail; // Tail sentinel node
    bool mtf;
};

LinkedList *ll_create(bool mtf) {
    LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));
    if (ll) {
        ll->length = 0;
        ll->head = node_create("Head", NULL);
        ll->tail = node_create("Tail", NULL);
        if (ll->head && ll->tail) {
            ll->tail->prev = ll->head;
            ll->head->next = ll->tail;
        }
        ll->mtf = mtf;
    }
    return ll;
}

void ll_delete(LinkedList **ll) {
    if (*ll && (*ll)->head && (*ll)->tail) {
        Node *curr = (*ll)->head;
        while (curr != (*ll)->tail) {
            curr = curr->next;
            node_delete(&curr->prev);
        }
        node_delete(&curr);
        curr = NULL;
        free(*ll);
        *ll = NULL;
    }
    return;
}

uint32_t ll_length(LinkedList *ll) {
    return ll->length;
}

Node *ll_lookup(LinkedList *ll, char *oldspeak) {
    seeks += 1;
    Node *ret_val = NULL;
    Node *curr = ll->head->next;
    // traverse the ll
    while (curr != ll->tail) {
        // if the word is the same then set ret_val to curr
        if (!strcmp(curr->oldspeak, oldspeak)) {
            ret_val = curr;
            break;
        } else {
            // else keep traversing
            links += 1;
            curr = curr->next;
        }
    }
    // if mtf is enabled then mtf
    if (ll->mtf && ret_val) {
        ret_val->prev->next = ret_val->next;
        ret_val->next->prev = ret_val->prev;
        ret_val->prev = ll->head;
        ret_val->next = ll->head->next;
        ll->head->next = ret_val;
        ret_val->next->prev = ret_val;
    }
    return ret_val;
}

void ll_insert(LinkedList *ll, char *oldspeak, char *newspeak) {
    // insert a node
    if (!ll_lookup(ll, oldspeak)) {
        Node *n = node_create(oldspeak, newspeak);
        n->prev = ll->head;
        n->next = ll->head->next;
        ll->head->next = n;
        n->next->prev = n;
        ll->length += 1;
    }
    return;
}

void ll_print(LinkedList *ll) {
    Node *curr = ll->head->next;
    while (curr->next != NULL) {
        node_print(curr);
        curr = curr->next;
    }
    return;
}
