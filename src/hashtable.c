#include <stdio.h>
#include <stdlib.h>
#include "../include/_hashtable.h"
#include "../include/hashtable.h"

static void _throw_error(char *msg);

HashTable *ht_init() {}

static void _throw_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

