#include <stdio.h>
#include "../include/hashtable.h"

int main() {
    HashTable *ht = ht_init();
    ht_put(ht, "a", 1);
    printf("{a : %d}\n", ht_get(ht, "a"));
}

