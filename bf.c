#include "bf.h"

#include "speck.h"

#include <stdio.h>
#include <stdlib.h>

struct BloomFilter {
    uint64_t primary[2]; // Primary hash function salt
    uint64_t secondary[2]; // Secondary hash function salt
    uint64_t tertiary[2]; // Tertiary hash filter salt
    BitVector *filter;
};

BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));
    if (bf) {
        bf->primary[0] = 0x5adf08ae86d36f21;
        bf->primary[1] = 0xa267bbd3116f3957;

        bf->secondary[0] = 0x419d292ea2ffd49e;
        bf->secondary[1] = 0x09601433057d5786;

        bf->tertiary[0] = 0x50d8bb08de3818df;
        bf->tertiary[1] = 0x4deaae187c16ae1d;

        bf->filter = bv_create(size);
        if (!bf->filter) {
            bv_delete(&bf->filter);
            bf = NULL;
        }
    }
    return bf;
}

void bf_delete(BloomFilter **bf) {
    if (*bf && (*bf)->filter) {
        bv_delete(&(*bf)->filter);
        free(*bf);
        *bf = NULL;
    }
    return;
}

uint32_t bf_size(BloomFilter *bf) {
    return bv_length(bf->filter);
}

void bf_insert(BloomFilter *bf, char *oldspeak) {
    // hash each number and flip the corresponding bit
    if (oldspeak) {
        uint32_t index = hash(bf->primary, oldspeak) % bv_length(bf->filter);
        bv_set_bit(bf->filter, index);
        index = hash(bf->secondary, oldspeak) % bv_length(bf->filter);
        bv_set_bit(bf->filter, index);
        index = hash(bf->tertiary, oldspeak) % bv_length(bf->filter);
        bv_set_bit(bf->filter, index);
    }
    return;
}

bool bf_probe(BloomFilter *bf, char *oldspeak) {
    // If all all the bits are flipped return true
    if (oldspeak && bv_get_bit(bf->filter, hash(bf->primary, oldspeak) % bv_length(bf->filter))
        && bv_get_bit(bf->filter, hash(bf->secondary, oldspeak) % bv_length(bf->filter))
        && bv_get_bit(bf->filter, hash(bf->tertiary, oldspeak) % bv_length(bf->filter))) {
        return true;
    } else {
        return false;
    }
}

uint32_t bf_count(BloomFilter *bf) {
    // traverse the filter and count how many are flipped
    uint32_t ret_val = 0;
    for (uint32_t i = 0; i < bv_length(bf->filter); i++) {
        if (bv_get_bit(bf->filter, i)) {
            ret_val += 1;
        }
    }
    return ret_val;
}

void bf_print(BloomFilter *bf) {
    bv_print(bf->filter);
    return;
}
