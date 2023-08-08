#include "bv.h"

#include <stdio.h>
#include <stdlib.h>

struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

BitVector *bv_create(uint32_t length) {
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector));
    if (bv) {
        bv->length = length;
        bv->vector = (uint8_t *) calloc((length / 8 + (length % 8 != 0)), sizeof(uint8_t));
        if (!bv->vector) {
            free(bv);
            bv = NULL;
        }
    }
    return bv;
}

void bv_delete(BitVector **bv) {
    if (*bv && (*bv)->vector) {
        free((*bv)->vector);
        free(*bv);
        *bv = NULL;
    }
    return;
}

uint32_t bv_length(BitVector *bv) {
    return bv->length;
}

void bv_set_bit(BitVector *bv, uint32_t i) {
    if (i < bv->length) {
        bv->vector[i / 8] |= ((uint8_t) 0x01 << (uint8_t) i % 8);
    }
    return;
}

void bv_clr_bit(BitVector *bv, uint32_t i) {
    if (i < bv->length) {
        bv->vector[i / 8] &= ~((uint8_t) 0x01 << (uint8_t) i % 8);
    }
    return;
}

uint8_t bv_get_bit(BitVector *bv, uint32_t i) {
    if (i < bv->length) {
        return (bv->vector[i / 8] & ((uint8_t) 0x01 << (uint8_t) i % 8)) >> (uint8_t) i % 8;
    } else {
        return 0;
    }
}

void bv_print(BitVector *bv) {
    printf("Bit Vector length: %u Bits\n", bv->length);
    for (uint32_t i = 0; i < bv->length; i++) {
        printf("%2u ", bv_get_bit(bv, i));
    }
    printf("\n");
    for (uint32_t i = 0; i < bv->length; i++) {
        printf("%2u ", i);
    }
    printf("\n");
    return;
}
