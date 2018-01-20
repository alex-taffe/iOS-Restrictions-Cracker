/// \file table.c
/// \brief A generic hash table data structure.
/// Author: Alex Taffe (RIT CS)

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "table.h"

/// Create a new hash table.
/// @param hash The key's hash function
/// @param equals The key's equal function for comparison
/// @param print A print function for the key/value, used for dump debugging
/// @exception Assert fails if can't allocate space
/// @return A newly created table
Table* create(long (*hash)(void* key),
              bool (*equals)(void* key1, void* key2),
              void (*print)(void* key1, void* key2)){
    Table *table = malloc(sizeof(Table));

    assert(table != NULL);

    table->hash = hash;
    table->equals = equals;
    table->print = print;
    table->table = calloc(INITIAL_CAPACITY, sizeof(Entry*) );

    assert(table->table != NULL);

    table->size = 0;
    table->capacity = INITIAL_CAPACITY;
    table->collisions = 0;
    table->rehashes = 0;
    return table;
}

/// Destroy a table
/// @param t The table to destroy
void destroy(Table* t){
    size_t i;
    //iterate over the table and free each item
    for(i = 0; i < t->capacity; i++)
        free(t->table[i]);
    //free the table
    free(t->table);
    //free the table struct
    free(t);
}

/// Print out information about the hash table (size,
/// capacity, collisions, rehashes).  If full is
/// true, it will also print out the entire contents of the hash table,
/// using the registered print function with each non-null entry.
/// @param t The table to display
/// @param full Do a full dump of entire table contents
void dump(Table* t, bool full){
    //print out table info
    printf("Size: %zu\n", t->size);
    printf("Capacity: %zu\n", t->capacity);
    printf("Collisions: %zu\n", t->collisions);
    printf("Rehashes: %zu\n", t->rehashes);
    //see if we should do a full print
    if(full){
        size_t i;
        //iterate over the whole table
        for(i = 0; i < t->capacity; i++){
            //print the current item
            printf("%zu: ", i);

            //make sure the slot isnt null
            if(t->table[i] != NULL){
                //print the key value pair
                printf("(");
                t->print(t->table[i]->key,t->table[i]->value);
                printf(")\n");
            }
            else{
                printf("null\n");
            }

        }
    }
}

/// Get the index of a key in the hashmap
/// It will return the index of the Entry
/// struct if found, -1 otherwise
/// @param t The table to sarch
/// @param key the key to search for
/// @return the index of the element containing the key, -1 otherwise
static size_t indexOfKey(Table* t, void* key){
    //get the location
    size_t location = t->hash(key) % t->capacity;
    bool hasLooped = false;
    size_t i;

    //begin looping
    for(i = location; i < t->capacity; i++){
        //make sure we dont hit a null spot and that we havent gone through
        //the whole table already
        if(t->table[i] == NULL || (i == location && hasLooped))
            break;

        //we found our object
        if(t->equals(t->table[i]->key,key))
            return i;

        //add a collision
        //this will increment for both get and has
        t->collisions++;
        //we are at the end, go back to the beginning
        if(i == t->capacity - 1){
            hasLooped = true;
            i = -1;
        }
    }
    return i;
}

/// Get the value associated with a key from the table.  This function
/// uses the registered hash function to locate the key, and the
/// registered equals function to check for equality.
/// @pre The table must have the key, or else it will assert fail
/// @param t The table
/// @param key The key
/// @return The associated value of the key
void* get(Table* t, void* key){
    //get the index in the array of the key
    long keyIndex = indexOfKey(t, key);
    //make sure it exists
    assert(t->table[keyIndex] != NULL);
    //return its value
    return t->table[keyIndex]->value;
}

/// Check if the table has a key.  This function uses the registered hash
/// function to locate the key, and the registered equals function to
/// check for equality.
/// @param t The table
/// @param key The key
/// @return Whether the key exists in the table.
bool has(Table* t, void* key){
    return t->table[indexOfKey(t,key)] != NULL;
}

/// Get the collection of keys from the table.  This function allocates
/// space to store the keys, which the caller is responsible for freeing.
/// @param t The table
/// @exception Assert fails if can't allocate space
/// @return A dynamic array of keys
void** keys(Table* t){
    //make the sure table size isnt 0
    assert(t->size != 0);

    //make an array of the key size
    void **keyList = malloc(sizeof(void*) * t->size);
    assert(keyList != NULL);

    size_t i, j;
    //iterate over the table and find all keys
    for(i = 0, j = 0; i < t->capacity; i++)
        if(t->table[i] != NULL)
            keyList[j++] = t->table[i]->key;

    return keyList;
}

/// Resizes a passed table based on
/// the RESIZE_FACTOR
/// @param t The table to be resized
/// @exception Assert fails if can't allocate space or resize table
static void resize(Table *t){
    Entry **oldValues = malloc(sizeof(Entry *) * t->size);

    assert(oldValues != NULL);

    size_t i, j;
    //get all of the old values in the list
    for(i = 0, j=0; i < t->capacity; i++)
        if(t->table[i])
            oldValues[j++] = t->table[i];


    //increase the number of rehashes and up capacity
    t->rehashes++;
    t->capacity *= RESIZE_FACTOR;

    //free the old table
    free(t->table);

    //make a new one
    t->table = calloc(t->capacity, sizeof(Entry *));

    assert(t->table != NULL);

    //store the old size and reset table size
    size_t oldSize = t->size;
    t->size = 0;

    //add the old values in
    for(i = 0; i < oldSize; i++){
        size_t location = indexOfKey(t, oldValues[i]->key);
        t->table[location] = oldValues[i];
        t->size++;
    }


    //free the old values array
    free(oldValues);
}


/// Add a key value pair to the table, or update an existing key's value.
/// This function uses the registered hash function to locate the key,
/// and the registered equals function to check for equality.
/// @param t The table
/// @param key The key
/// @param value The value
/// @exception Assert fails if can't allocate space
/// @return The old value in the table, if one exists.
void* put(Table* t, void* key, void* value){
    //see if the key already exists and reset collisions afterwards
    //size_t oldCollisions = t->collisions;
    size_t keyIndex = indexOfKey(t, key);
    //t->collisions = oldCollisions;
    if(t->table[keyIndex] != NULL){
        //it does, return the old value and store the new one
        void *oldValue = t->table[keyIndex]->value;
        t->table[keyIndex]->value = value;
        return oldValue;
    }
    else{
        //new key value pair
        t->table[keyIndex] = malloc(sizeof(Entry));
        assert(t->table[keyIndex] != NULL);
        t->table[keyIndex]->key = key;
        t->table[keyIndex]->value = value;

        //see if we need to resize
        t->size++;
        if((float)t->size / (float)t->capacity >= LOAD_THRESHOLD)
            resize(t);

        return NULL;
    }
}

/// Get the collection of values from the table.  This function allocates
/// space to store the values, which the caller is responsible for freeing.
/// @param t The table
/// @exception Assert fails if can't allocate space
/// @return A dynamic array of values
void** values(Table* t){
    //make the sure table size isnt 0
    assert(t->size != 0);

    //make an array of the key size
    void **valueList = malloc(sizeof(void*) * t->size);
    assert(valueList != NULL);

    size_t i, j;
    //iterate over the table and find all keys
    for(i = 0, j = 0; i < t->capacity; i++)
        if(t->table[i] != NULL)
            valueList[j++] = t->table[i]->value;

    //return the list
    return valueList;
}

