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
#include "b64.h"
#include "fastpbkdf2.h"

#define MAX_PASSCODE 9999.0
#define OUTPUT_LENGTH 20

long cores;
struct s_threadId {
    pthread_mutex_t   mtx;    /* mutex & condition to allow main thread to
                               wait for the new thread to  set its TID */
    pthread_cond_t    cond;   /* '' */
    pid_t             id;     /* to hold new thread's TID */
    int               ready;  /* to indicate when 'id' is valid, in case
                               the condition wait gets interrupted */
    int               numThread; /* used to hold the number of the thread */
};

const char *globalHash;
char *globalSalt;
struct timeval start, end;


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
   
    int threadNumber = thId->numThread;
    
    int bottom = 0;
    if(threadNumber != 0)
        bottom = (ceil(MAX_PASSCODE / cores) * (threadNumber));
    int top = (ceil(MAX_PASSCODE / cores) * (threadNumber + 1)) - 1;
    
    uint8_t out[OUTPUT_LENGTH];
    char *b64;
    char *passcode = (char*)malloc(5 * sizeof(char));
    
    for (int i = bottom; i <= top; i++) {
        
        sprintf(passcode, "%04i", i);
        
        fastpbkdf2_hmac_sha1(passcode, 4, globalSalt, 4, 1000, out, OUTPUT_LENGTH);
        
        b64 = b64_encode(out, OUTPUT_LENGTH);
        
        if(strcmp(globalHash, b64) == 0){
            gettimeofday(&end, NULL);
            double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
            printf("The code is: %s\nCalculated in %.2f seconds\n", passcode, delta);
			
			free(passcode);

            exit(0);
        }
    }
    
}

void crackCode(const char *hash, const char *salt, char *code, char *error){
    gettimeofday(&start, NULL);
    
    globalHash = hash;
    globalSalt = b64_decode(salt, 8);
    
    cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cracking passcode with %li threads\n", cores);
    
    pthread_t thread[cores]; /* reused for each thread, since they run 1 at a time */
    
    /* struct to pass back TID */
    struct s_threadId threadId;
    pthread_cond_init(&threadId.cond, NULL);  /* init condition */
    pthread_mutex_init(&threadId.mtx, NULL);  /* init mutex */
    
    
    int err;
    
    for(int i = 0; i < cores; i++){
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
            /* Now we have the TID... */
            //printf("%d %d\n", i, threadId.id);
            //printf("I just created thread %d\n", i);
        }
        /* ..and unlock the mutex when done. */
        pthread_mutex_unlock(&threadId.mtx);
    }
    for(int i = 0; i < cores; i++){
        pthread_join(thread[i], NULL);
    }
    gettimeofday(&end, NULL);
    double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
    printf("Code not found. Verify that you have entered the correct hash and salt\nTook %.2f seconds\n", delta);
}
