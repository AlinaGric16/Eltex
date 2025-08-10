#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define SHOPS 5
#define BUYERS 3

extern pthread_mutex_t shops_mutex[SHOPS];
extern int shops[SHOPS];
extern int requirements[BUYERS];

typedef struct {
    int buyer_id;
    int* shops;
    int* requirements;
} BuyerData;

void* shopping(void *args);
void* supply(void* args);

#endif