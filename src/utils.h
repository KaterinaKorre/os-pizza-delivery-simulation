#ifndef PIZZA_UTILS_H
#define PIZZA_UTILS_H

void errorThread_create(int t);
void errorThread_join(int t);
void errorMutex_lock(int t);
void errorMutex_unlock(int t);
void errorStartTime(int t);
void errorEndTime(int t);
void errorCondWait(int t);
void errorCondSignal(int t);

#endif //PIZZA_UTILS_H