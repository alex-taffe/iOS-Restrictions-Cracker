/// \file table.h
/// \brief A generic hash table data structure.

#ifndef TABLE_H
#define TABLE_H

#include <stdbool.h>    // bool
#include <stddef.h>     // size_t

/// Initial capacity of table upon creation
#define INITIAL_CAPACITY 16

/// The load at which the table will rehash
#define LOAD_THRESHOLD 0.75

/// The table size will double upon each rehash
#define RESIZE_FACTOR 2

/// The "buckets" in the table
typedef struct Entry_t {
    void* key;     ///< key must be "hashable"
    void* value;   ///< value can be any type
} Entry;

/// The hash table
typedef struct Table_t {
    Entry** table;    ///< table of Entry pointers
    size_t size;      ///< number of entries in table
    size_t capacity;  ///< total capacity of table
    long (*hash)(void* key);  ///< hash function for key
    bool (*equals)(void *key1, void* key2); ///< equals function for key comparison
    void (*print)(void *key, void* value);  ///< print function for table dump debug
    size_t collisions;      ///< number of collisions throughout life of hash table
    size_t rehashes;        ///< number of rehashes throughout life of hash table
} Table;

Table* create(long (*hash)(void* key),
              bool (*equals)(void* key1, void* key2),
              void (*print)(void* key1, void* key2));

void destroy(Table* t);

void dump(Table* t, bool full);

void* get(Table* t, void* key);

bool has(Table* t, void* key);

void** keys(Table* t);

void* put(Table* t, void* key, void* value);

void** values(Table* t);

#endif // TABLE_H
