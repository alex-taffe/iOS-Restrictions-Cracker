//
//  crack.c
//  iOS Restrictions Cracker
//
//  Created by Alex Taffe on 9/3/17.
//  Copyright © 2017 Alex Taffe. All rights reserved.
//

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
#include "b64.h"
#include "fastpbkdf2.h"
#include "hashtable.h"

#define MAX_PASSCODE 9999.0
#define OUTPUT_LENGTH 20

//helps with threading
struct s_threadId {
    pthread_mutex_t   mtx;    /* mutex & condition to allow main thread to
                               wait for the new thread to  set its TID */
    pthread_cond_t    cond;   /* '' */
    pid_t             id;     /* to hold new thread's TID */
    int               ready;  /* to indicate when 'id' is valid, in case
                               the condition wait gets interrupted */
    int               numThread; /* used to hold the number of the thread */
};

//representation of a code to be cracked
typedef struct{
    char *hash;
    unsigned char *salt;
}HashItem;

long numCores; //number of logical (including hyper threaded) cores in the system
struct timeval start, end;

hashtable_t *crackedItems; //already cracked items
HashItem **itemsToCrack; //items we still need to crack
size_t numItemsToCrack; //how many items there are to be cracked



/**
 * @brief Attempts to crack a section of a code. For example, on a 10 core system
 *        on thread 0, we will try codes 0000-0999
 * @param arg : A pointer to a s_threadId instance
 * @return NULL
 */
void* crackSection(void* arg){
    struct s_threadId *thId = arg;

    /* Lock mutex... */
    pthread_mutex_lock(&thId->mtx);

    /* Get and save TID and ready flag.. */
    thId->id = syscall(SYS_gettid);
    thId->ready = 1;
    /* ..and signal main thread that we're ready */
    pthread_cond_signal(&thId->cond);

    /* ..then unlock when we're done. */
    pthread_mutex_unlock(&thId->mtx);

    //see what thread we are
    int threadNumber = thId->numThread;

    //get the range of codes we're going to try cracking on this thread
    int bottom = 0;
    if(threadNumber != 0)
        bottom = (ceil(MAX_PASSCODE / numCores) * (threadNumber));
    int top = (ceil(MAX_PASSCODE / numCores) * (threadNumber + 1)) - 1;

    uint8_t out[OUTPUT_LENGTH]; //holder for hmac_sha1 output
    char *passcode = (char*)malloc(5 * sizeof(char)); //used to store each passcode
    assert(passcode != NULL); //make sure our malloc worked

    //the item we're going to crack each iteration
    HashItem *itemToCrack;

    for(size_t i = 0; i < numItemsToCrack; i++){

        //get the item we're going to crack
        itemToCrack = itemsToCrack[i];

        //the hash for this item (used to reduce memory dereferences)
        char *hash = itemToCrack->hash;

        //the salt for this item (used to reduce memory dereferences)
        unsigned char *salt = itemToCrack->salt;

        //loop within our range of items to crack. Also see if this code has already been found
        for (int j = bottom; j <= top && ht_get(crackedItems, itemToCrack, sizeof(HashItem), NULL) == NULL; j++) {
            //get string representation of the code we need
            sprintf(passcode, "%04i", j);

            //hash that code with the designated salt
            fastpbkdf2_hmac_sha1((unsigned char *)passcode, 4, salt, 4, 1000, out, OUTPUT_LENGTH);

            //see if our code matches
            if(strcmp(hash, b64_encode(out, OUTPUT_LENGTH)) == 0){
                //it does, so add it as completed to the hashmap
                ht_set(crackedItems, itemToCrack, sizeof(HashItem), strdup(passcode), strlen(passcode) + 1);
            }
        }

    }

    //we're done, exit the thread
    pthread_exit(NULL);
}

/**
 * @brief Attempts to crack a list of codes passed in
 * @param hashes : An array of pointers of the hashes to be cracked
 * @param salts  : An array of pointers of the salts to be cracked
 */
void crackCodes(char **hashes, char **salts){
    //get the current time of the day so we can time program execution
    gettimeofday(&start, NULL);

    //create a hash table for our results
    crackedItems = ht_create(0, SIZE_MAX, NULL);

    //see how many items we actually need to crack
    numItemsToCrack = sizeof(hashes) / sizeof(char *);

    //create an array to hold the items we need to crack
    itemsToCrack = malloc(sizeof(HashItem) * numItemsToCrack);

    //loop over all the items we need to crack and create structs to represent them
    //then add them to the array
    for(size_t i = 0; i < numItemsToCrack; i++){
        HashItem *item = malloc(sizeof(HashItem));
        item->hash = strdup(hashes[i]);
        item->salt = b64_decode(salts[i], 8);

        itemsToCrack[i] = item;
    }

    //get the number of cores the system has
    numCores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cracking passcode with %li threads\n", numCores);

    pthread_t thread[numCores]; /* reused for each thread, since they run 1 at a time */

    /* struct to pass back TID */
    struct s_threadId threadId;
    pthread_cond_init(&threadId.cond, NULL);  /* init condition */
    pthread_mutex_init(&threadId.mtx, NULL);  /* init mutex */

    //create all our threads
    for(int i = 0; i < numCores; i++){
        /* lock mutex *before* creating the thread, to make the new thread
         wait until we're ready before signaling us */
        pthread_mutex_lock(&threadId.mtx);

        /* clear ready flag before creating each thread */
        threadId.ready = 0;
        threadId.numThread = i;
        /* create threads and pass address of struct as argument */
        if (pthread_create(&thread[i], NULL, crackSection, &threadId)) {
            printf("pthread error!\n");
        } else {
            /* Wait on the condition until the ready flag is set */
            while (!threadId.ready) {
                pthread_cond_wait(&threadId.cond, &threadId.mtx);
            }
        }
        /* ..and unlock the mutex when done. */
        pthread_mutex_unlock(&threadId.mtx);
    }
    //make sure the program doesn't end prematurely
    for(int i = 0; i < numCores; i++){
        pthread_join(thread[i], NULL);
    }

    //see how many codes we actually found
    size_t numberFoundCodes = ht_count(crackedItems);

    //get the end of the execution time
    gettimeofday(&end, NULL);
    double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

    //print out info
    printf("Found %lu out of %lu codes\nTook %.2f seconds\nResults:\n\n", numberFoundCodes, numItemsToCrack, delta);

    //get all keys
    linked_list_t *allCodes = ht_get_all_keys(crackedItems);

    //loop over all the codes we found and print them out
    for(size_t i = 0; i < numberFoundCodes; i++){
        HashItem *item = ((hashtable_key_t*)list_pop_value(allCodes))->data;
        char *value = ht_get(crackedItems, item, sizeof(HashItem), NULL);
        printf("\tHash: %s, Result: %s\n", item->hash, value);
    }

    //clean up memory
    ht_destroy(crackedItems); //destroy the hash table
}
