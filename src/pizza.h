#ifndef PIZZA_PIZZA_H
#define PIZZA_PIZZA_H

#define Ntel 2
#define Ncook 2
#define Noven 10
#define Ndeliverer 10 
#define Torderlow 1
#define Torderhigh 5
#define Norderlow 1
#define Norderhigh 5
#define Pm 3.5
#define Pp 2.5
#define Ps 4
#define Tpaymentlow 1
#define Tpaymenthigh 3
#define Pfail 0.5
#define Cm 10
#define Cp 11
#define Cs 12
#define Tprep 1
#define Tbake 10
#define Tpack 1
#define Tdellow 5
#define Tdelhigh 15
#define BILLION  1000000000L;

typedef struct thread_arguments{
     int id;
     int seed;
} thread_arguments;

typedef struct order_info{
    int id;
    int seed;
    int margarita;
    int peperoni;
    int special;

} order_info;

typedef struct time_measurements {
    struct timespec packaging_time_start; // start time of time space <X>
    struct timespec delivery_time_start; // start time of time space <Y>
} time_measurements;

typedef struct delivery_info{
    struct thread_arguments thread_info; // thread information
    struct timespec t_coldPizza_start; // start time of time space when the pizza finished baking
} delivery_info;

struct order_info order(void *args);

struct delivery_info preparation(struct order_info args, struct time_measurements timeMeasurements);

void delivery(struct delivery_info deliveryArgs, struct time_measurements timeMeasurements);

void *run_thread(void *args);

#endif // PIZZA_PIZZA_H