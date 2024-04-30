#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

#include <stddef.h>

typedef struct ht_entry {
    char *key;
    int val;
} Entry;

struct hashtable {
    Entry **entries;
    size_t size;
    size_t capacity;  // load factor = size / capacity
};

#endif

