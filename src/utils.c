#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void errorThread_create(int t) {
    if(t!=0){
        printf("Failed to create thread");
        exit(-1);
    }
}
void errorThread_join(int t) {
    if(t!=0){
        printf("Failed to join thread");
        exit(-1);
    }
}
void errorMutex_lock(int t){
    if(t!=0){
        printf("Mutex failed to lock");
        exit(-1);
    }
}
void errorMutex_unlock(int t){
    if(t!=0){
        printf("Mutex failed to unlock");
        exit(-1);
    }
}

void errorStartTime(int t){
    if(t==-1){
        printf("Error in start in clock_gettime");
        exit(-1);
    }
}

void errorEndTime(int t) {
    if(t == -1) {
        printf("Error in end in clock_gettime");
        exit(-1);
    }
}

void errorCondWait(int t) {
    if (t != 0) {
        printf("Error in condition wait");
        exit(-1);
    }
}
void errorCondSignal(int t){
    if (t!=0){
        printf("Error in condition signal");
        exit(-1);
    }
}
