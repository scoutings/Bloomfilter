#include "bf.h"
#include "bv.h"
#include "ht.h"
#include "ll.h"
#include "messages.h"
#include "node.h"
#include "parser.h"

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define OPTIONS "ht:f:ms"

#define BLOCK    4096
#define BADSPEAK "badspeak.txt"
#define NEWSPEAK "newspeak.txt"
#define WORD     "[a-zA-Z0-9_]+(('|-)[a-zA-Z0-9_]+)*"

static void print_helper() {
    printf(
        "SYNOPSIS\n  A word filtering program for the GPRSC.\n  Filters out and reports bad words "
        "parsed from stdin.\n\nUSAGE\n  ./banhammer [-hsm] [-t size] [-f size]\n\nOPTIONS\n  -h    "
        "       Program usage and help.\n  -s           Print program statistics.\n  -m           "
        "Enable move-to-front rule.\n  -t size      Specify hash table size (default: 10000).\n  "
        "-f size      Specify Bloom filter size (default: 2^20).\n");
    return;
}

static void print_statistics(uint64_t seeks_, uint64_t links_, uint32_t ht_count, uint32_t ht_size,
    uint32_t bf_count, uint32_t bf_size) {
    printf("Seeks: %lu\nAverage seek length: %.6f\nHash table load: %.6f%%\nBloom filter load: "
           "%.6f%%\n",
        seeks_, (double) links_ / (double) seeks_, 100.0 * ((double) ht_count / (double) ht_size),
        100.0 * ((double) bf_count / (double) bf_size));
}

static bool validate_strtol(int32_t convertion_val, char *inval) {
    if (inval[0] != '\0' || errno == ERANGE || errno == EINVAL || convertion_val <= 0) {
        return false;
    } else {
        return true;
    }
}

int main(int argc, char **argv) {
    int opt;
    enum { HASH, BLOOM_FILTER };
    enum { MTF, STATS };

    char *inval;
    uint32_t size_opt[2] = { 10000, 1048576 };
    bool options[2] = { false, false };

    // Parse out program arguments
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        int32_t convertion;
        switch (opt) {
        case 'h': print_helper(); exit(0);
        case 't':
            errno = 0;
            convertion = strtol(optarg, &inval, 10);
            if (!validate_strtol(convertion, inval)) {
                printf("Invalid hash table size.\n");
                exit(1);
            }
            size_opt[HASH] = (uint32_t) convertion;
            break;
        case 'f':
            errno = 0;
            convertion = strtol(optarg, &inval, 10);
            if (!validate_strtol(convertion, inval)) {
                printf("Invalid Bloom filter size.\n");
                exit(1);
            }
            size_opt[BLOOM_FILTER] = (uint32_t) convertion;
            break;
        case 'm': options[MTF] = true; break;
        case 's': options[STATS] = true; break;
        }
    }

    // Initialize bf and ht
    BloomFilter *bf = bf_create(size_opt[BLOOM_FILTER]);
    HashTable *ht = ht_create(size_opt[HASH], options[MTF]);

    // Read badspeak and insert in bf and ht
    FILE *badspeak = fopen(BADSPEAK, "r");
    char badspeak_str[BLOCK] = { 0 };
    while (fscanf(badspeak, "%s\n", badspeak_str) != EOF) {
        bf_insert(bf, badspeak_str);
        ht_insert(ht, badspeak_str, NULL);
    }
    fclose(badspeak);

    // Read newspeak and insert in bf and ht
    FILE *newspeak = fopen(NEWSPEAK, "r");
    char oldspeak_str[BLOCK] = { 0 };
    char newspeak_str[BLOCK] = { 0 };
    while (fscanf(newspeak, "%s %s\n", oldspeak_str, newspeak_str) != EOF) {
        bf_insert(bf, oldspeak_str);
        ht_insert(ht, oldspeak_str, newspeak_str);
    }
    fclose(newspeak);

    // Compile regex
    regex_t re;
    if (regcomp(&re, WORD, REG_EXTENDED)) {
        fprintf(stderr, "Failed to compile regex.\n");
        bf_delete(&bf);
        ht_delete(&ht);
        exit(1);
    }

    // Using linked list as we do not know the length of these lists
    LinkedList *ll_badspeak = ll_create(false);
    LinkedList *ll_oldspeak = ll_create(false);

    // Read each word and store the incorrect words in their repsective linked list
    char *word_str = NULL;
    while ((word_str = next_word(stdin, &re)) != NULL) {
        for (int i = 0; word_str[i]; i++) {
            word_str[i] = tolower((unsigned char) word_str[i]);
        }
        if (bf_probe(bf, word_str)) {
            Node *correction = ht_lookup(ht, word_str);
            if (correction) {
                if (correction->newspeak) {
                    ll_insert(ll_oldspeak, correction->oldspeak, correction->newspeak);
                } else {
                    ll_insert(ll_badspeak, correction->oldspeak, correction->newspeak);
                }
            }
        }
    }
    clear_words();
    regfree(&re);

    // Print statistics if enabled or print the message and the missused words
    if (options[STATS]) {
        print_statistics(seeks, links, ht_count(ht), ht_size(ht), bf_count(bf), bf_size(bf));
    } else {
        if (ll_length(ll_badspeak) > 0 && ll_length(ll_oldspeak) > 0) {
            printf("%s", mixspeak_message);
        } else {
            if (ll_length(ll_badspeak) > 0) {
                printf("%s", badspeak_message);
            }
            if (ll_length(ll_oldspeak) > 0) {
                printf("%s", goodspeak_message);
            }
        }
        ll_print(ll_badspeak);
        ll_print(ll_oldspeak);
    }

    // delete
    ll_delete(&ll_badspeak);
    ll_delete(&ll_oldspeak);
    bf_delete(&bf);
    ht_delete(&ht);
    return 0;
}
