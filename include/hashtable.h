#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>

typedef struct hashtable HashTable;
extern HashTable *ht_init();
extern size_t ht_size(HashTable *ht);
extern int ht_is_empty(HashTable *ht);
extern int ht_get(HashTable *ht, const char *key);
extern void ht_put(HashTable *ht, const char *key, const int val);
extern int ht_remove(HashTable *ht, const char *key);
extern void ht_del(HashTable *ht);

#endif

