#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *stringdup(char *str) {
    size_t size_of = strlen(str) + 1;
    char *ret_val = (char *) malloc(size_of);
    memcpy(ret_val, str, size_of);
    return ret_val;
}

Node *node_create(char *oldspeak, char *newspeak) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        n->next = n->prev = NULL;
        n->oldspeak = n->newspeak = NULL;
        if (oldspeak) {
            n->oldspeak = stringdup(oldspeak);
        }
        if (newspeak) {
            n->newspeak = stringdup(newspeak);
        }
    }
    return n;
}

void node_delete(Node **n) {
    if (*n) {
        if ((*n)->oldspeak) {
            free((*n)->oldspeak);
            (*n)->oldspeak = NULL;
        }
        if ((*n)->newspeak) {
            free((*n)->newspeak);
            (*n)->newspeak = NULL;
        }
        free(*n);
        *n = NULL;
    }
    return;
}

void node_print(Node *n) {
    if (n->newspeak == NULL) {
        printf("%s\n", n->oldspeak);
    } else {
        printf("%s -> %s\n", n->oldspeak, n->newspeak);
    }
    return;
}
