#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/hashtable.h"

#define INIT_SIZE 8  // TODO: Use prime table sizes
#define MAX_LOAD 0.5f
#define MIN_LOAD 0.1f
#define GROWTH_FACTOR 2
#define PRIME1 109345121
#define PRIME2 201326611

typedef struct ht_entry {
    const char *key;
    int val;
    int is_removable;
} Entry;

struct hashtable {
    Entry **entries;
    size_t size;
    size_t _size;  // this will not decrement after a ht_remove operation
    size_t capacity;  // load factor = _size / capacity
};

static int is_seeded = 0;

static void _throw_error(const char *msg);
static Entry *_ht_entry_init(const char *key, const int val);
static unsigned _hash(const char *key, unsigned p, size_t table_capacity);
static void _expand(HashTable *ht);
static void _shrink(HashTable *ht);

HashTable *ht_init() {
    if (!is_seeded) {
        srand(time(NULL));
        is_seeded = 1;
    }
    HashTable *ht = malloc(sizeof(*ht));
    if (ht == NULL)
        _throw_error("Could not allocate memory for HashTable");
    ht->entries = malloc(sizeof(*ht->entries) * INIT_SIZE);
    if (ht->entries == NULL)
        _throw_error("Could not allocate memory for initial HashTable Entries");
    for (size_t i = 0; i < INIT_SIZE; i++)
        ht->entries[i] = NULL;
    ht->size = 0;
    ht->_size = 0;
    ht->capacity = INIT_SIZE;
    return ht;
}

size_t ht_size(HashTable *ht) {
    return ht->size;
}

int ht_is_empty(HashTable *ht) {
    return ht_size(ht) == 0;
}

int ht_get(HashTable *ht, const char *key) {
    size_t i = 1;
    unsigned ht_idx = _hash(key, PRIME1, ht->capacity);
    while (ht->entries[ht_idx] != NULL && !ht->entries[ht_idx]->is_removable && i <= ht->_size) {  // is the third condition needed?
        if (!strcmp(ht->entries[ht_idx]->key, key))
            return ht->entries[ht_idx]->val;
        ht_idx = (_hash(key, PRIME1, ht->capacity) + i * _hash(key, PRIME2, ht->capacity)) % ht->capacity;  // TODO: the second hash function should be odd if the table capacity is even
        i++;
    }
    fprintf(stderr, "Entry with key '%s' not found in HashTable\n", key);
    exit(EXIT_FAILURE);
}

void ht_put(HashTable *ht, const char *key, const int val) {
    size_t i = 1;
    unsigned ht_idx = _hash(key, PRIME1, ht->capacity);
    while (ht->entries[ht_idx] != NULL && !ht->entries[ht_idx]->is_removable) {
        ht_idx = (_hash(key, PRIME1, ht->capacity) + i * _hash(key, PRIME2, ht->capacity)) % ht->capacity;
        i++;
    }
    ht->entries[ht_idx] = _ht_entry_init(key, val);
    ht->size++;
    ht->_size++;
    if ((float) ht->_size / ht->capacity >= MAX_LOAD)
        _expand(ht);
}

int ht_remove(HashTable *ht, const char *key) {
    size_t i = 1;
    unsigned ht_idx = _hash(key, PRIME1, ht->capacity);
    while (ht->entries[ht_idx] != NULL && !ht->entries[ht_idx]->is_removable && i <= ht->_size) {
        if (!strcmp(ht->entries[ht_idx]->key, key)) {
            int val = ht->entries[ht_idx]->val;
            ht->entries[ht_idx]->is_removable = 1;
            ht->size--;
            if (ht->size <= (int) (MIN_LOAD * ht->capacity))
                _shrink(ht);
            return val;
        }
        ht_idx = (_hash(key, PRIME1, ht->capacity) + i * _hash(key, PRIME2, ht->capacity)) % ht->capacity;
        i++;
    }
    fprintf(stderr, "Cannot remove entry with key '%s', as it does not exist\n", key);
    exit(EXIT_FAILURE);
}

void ht_del(HashTable *ht) {
    // free each entry's key?
    // free each entry
    // free ht
}

static void _throw_error(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

static Entry *_ht_entry_init(const char *key, const int val) {
    Entry *e = malloc(sizeof(*e));
    if (e == NULL)
        _throw_error("Could not allocate memory for HashTable Entry");
    e->key = key;
    e->val = val;
    e->is_removable = 0;
    return e;
}

static unsigned _compress(unsigned i, unsigned p, size_t m) {
    unsigned scale = rand() % p + 1;
    unsigned shift = rand() % p;
    return ((scale * i + shift) % p) % m;
}

static unsigned _hash(const char *key, unsigned p, size_t table_capacity) {
    unsigned h = 0;
    for (; *key != '\0'; key++) {
        h = (h << 5) | (h >> 27);  // cyclic shift
        h += *key;
    }
    return _compress(h, p, table_capacity);
}

static void _rehash_entry(HashTable *old_table, size_t entry_idx, Entry **new_table) {
    size_t new_table_capacity = old_table->capacity * GROWTH_FACTOR;
    size_t i = 1;
    unsigned ht_idx = _hash(old_table->entries[entry_idx]->key, PRIME1, new_table_capacity);
    while (new_table[ht_idx] != NULL) {
        ht_idx = (_hash(old_table->entries[entry_idx]->key, PRIME1, new_table_capacity) + i * _hash(old_table->entries[entry_idx]->key, PRIME2, new_table_capacity)) % new_table_capacity;
        i++;
    }
    new_table[ht_idx] = _ht_entry_init(old_table->entries[entry_idx]->key, old_table->entries[entry_idx]->val);
}

static void _expand(HashTable *ht) {
    Entry **new_table = malloc(sizeof(*new_table) * ht->capacity * GROWTH_FACTOR);
    for (size_t i = 0; i < ht->capacity; i++) {
        if (ht->entries[i] != NULL) {
            if (!ht->entries[i]->is_removable)
                _rehash_entry(ht, i, new_table);
            free(ht->entries[i]);
        }
    }
    free(ht->entries);
    ht->entries = new_table;
    ht->_size = ht->size;
    ht->capacity *= GROWTH_FACTOR;
}

static void _shrink(HashTable *ht) {
    Entry **new_table = malloc(sizeof(*new_table) * ht->capacity / GROWTH_FACTOR);
    for (size_t i = 0; i < ht->capacity; i++) {
        if (!ht->entries[i]->is_removable)
            _rehash_entry(ht, i, new_table);
        free(ht->entries[i]);
    }
    free(ht->entries);
    ht->entries = new_table;
    ht->_size = ht->size;
    ht->capacity /= GROWTH_FACTOR;
}

