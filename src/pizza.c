#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "utils.h"
#include "pizza.h"
#include <time.h>

//Initializing mutexes
pthread_mutex_t cook_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t oven_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tel_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ready_for_delivery_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t delivery_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t end_of_order_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_results_mtx = PTHREAD_MUTEX_INITIALIZER;
// Condition variables
pthread_cond_t cook_cond;
pthread_cond_t oven_cond;
pthread_cond_t tel_cond;
pthread_cond_t delivery_cond;

int margarita_sales = 0;            // number of sold margarita pizza
int peperoni_sales = 0;             // number of sold peperoni pizza
int special_sales = 0;              //  number of sold special pizza
int cooks = Ncook;                  // number of  available cooks
int ovens = Noven;                  // number of available ovens
int tel = Ntel;                     // number of available packers
int deliverers = Ndeliverer;        // number of available deliverers
int n_fail_orders = 0;              // number of orders that failed
double max_service_time = 0.0 ;     // maximum service time
double sum_service_time = 0.0 ;     // sum of service time
double max_cold_time = 0.0 ;        // maximum cold time
double sum_cold_time = 0.0 ;        // sum of cold time

struct order_info order(void *args) {
    struct order_info orders;       // each order has an id, number of margarita, peperoni and special pizza

    thread_arguments *order_args = (thread_arguments *) args;
    int seed = order_args->seed;
    int id = order_args->id;

    unsigned int temp_seed = (unsigned int) seed;   // Cast seed to unsigned int
    unsigned int *seed_ptr = &temp_seed;            // Create a pointer to the seed

    // Number of pizza is a random integer in range [Norderlow,Norderhigh].
    int n_pizza = (rand_r(seed_ptr) % (Norderhigh + 1 - Norderlow)) + Norderlow;

    int counter_margarita = 0;                     // counter for margarita pizza
    for (int i = 1; i <= n_pizza; i++) {
        if ((rand_r(seed_ptr) % 10) + 1 <= Pm) {  // A pizza has probability to be margarita: Pm
            counter_margarita++;
        }
    }
    int counter_peperoni = 0;                       // counter for peperoni pizza
    for (int i = 1; i <= n_pizza; i++) {
        if ((rand_r(seed_ptr) % 10) + 1 <= Pp) {  // A pizza has probability to be peperoni: Pp
            counter_peperoni++;
        }
    }
    int counter_special = 0;                        // counter for special pizza
    for (int i = 1; i <= n_pizza; i++) {
        if ((rand_r(seed_ptr) % 10) + 1 <= Ps) {  // A pizza has probability to be special: Ps
            counter_special++;
        }
    }
    // Payment takes a random time in range [Tpaymentlow,Tpaymenthigh].
    int time_to_pay = rand_r(seed_ptr) % (Tpaymenthigh + 1 - Tpaymentlow) + Tpaymentlow;
    sleep(time_to_pay);


    if (rand_r(seed_ptr) % 10 + 1 <= Pfail) {   // An order has probability to fail: Pfail
        printf("The order with number %d failed.\n", id);
        n_fail_orders++;
        // if the payment fails then the thread exits.
        free(args);
        pthread_exit(NULL);
    } else {
        // if the order succeeds, the order info are registered.
        orders.id = id;
        orders.margarita = counter_margarita;
        orders.peperoni = counter_peperoni;
        orders.special = counter_special;
        orders.seed = seed;

        printf("The order with number %d was registered.\n", id);

        margarita_sales += orders.margarita;
        peperoni_sales += orders.peperoni;
        special_sales += orders.special;
    }

    return orders;      // return the order info
}


struct delivery_info preparation(struct order_info args, struct time_measurements timeMeasurements) {

    struct thread_arguments threadInfo;
    int id = args.id;
    int number_of_pizza = args.margarita + args.peperoni + args.special;
    int check_unlock;
    int check_lock;
    int check_wait;
    int check_signal;
    struct delivery_info deliveryInfo;

    // A mutex where a cook undertakes an order
    check_lock = pthread_mutex_lock(&cook_mtx);
    errorMutex_lock(check_lock);

    while (cooks == 0) {    // if there's no available cook, the order waits.
        printf("There's no available cook for order with number %d. Please wait...\n", id);
        check_wait= pthread_cond_wait(&cook_cond, &cook_mtx);
        errorCondWait(check_wait);
    }

    printf("Order with number %d has started to prepare.\n", id);
    cooks--;

    check_unlock = pthread_mutex_unlock(&cook_mtx);
    errorMutex_unlock(check_unlock);

    int time_to_prep = Tprep * number_of_pizza;
    sleep(time_to_prep);        // Time to prepare every pizza.

    // A mutex where (number_of_pizza) ovens bake an order
    check_lock = pthread_mutex_lock(&oven_mtx);
    errorMutex_lock(check_lock);

    while (ovens < number_of_pizza) {       // if there's no available ovens, the order waits.
        printf("Order with number %d is waiting for the necessary number of available ovens.\n", id);
        check_wait=pthread_cond_wait(&oven_cond, &oven_mtx);
        errorCondWait(check_wait);
    }
    printf("Order with number %d is ready for the oven.\n", id);

    ovens -= number_of_pizza;
    cooks++;   // when ovens start to bake an order the cook is free and ready to undertake another order.
    check_signal = pthread_cond_signal(&cook_cond);
    errorCondSignal(check_signal);

    check_unlock = pthread_mutex_unlock(&oven_mtx);
    errorMutex_unlock(check_unlock);

    sleep(Tbake); // Time to bake all pizza at the same time.

    int check_start_bake = clock_gettime(CLOCK_REALTIME, &deliveryInfo.t_coldPizza_start);
    errorStartTime(check_start_bake);

    // A mutex where a tel takes an order
    check_lock = pthread_mutex_lock(&tel_mtx);
    errorMutex_lock(check_lock);

    while (tel == 0) { // if there's no available tel, the order waits.
        printf("There's no available tel for order with number %d . Please wait...\n", id);
        check_wait=pthread_cond_wait(&tel_cond, &tel_mtx);
        errorCondWait(check_wait);
    }
    printf("Order with number %d is ready for tel.\n", id);

    tel--;

    check_unlock = pthread_mutex_unlock(&tel_mtx);
    errorMutex_unlock(check_unlock);

    sleep(Tpack * number_of_pizza); // Time to pack every pizza.

    // A mutex where the packing finishes and the order is ready for delivery
    check_lock = pthread_mutex_lock(&ready_for_delivery_mtx);
    errorMutex_lock(check_lock);

    struct timespec finish_x;
    int check_stop_x = clock_gettime(CLOCK_REALTIME, &finish_x);
    errorEndTime(check_stop_x);

    double elapsed = (finish_x.tv_sec - timeMeasurements.packaging_time_start.tv_sec) +
                     (double) (finish_x.tv_nsec - timeMeasurements.packaging_time_start.tv_nsec) / BILLION;

    printf("The order with number %d has finished the preparation in %.2lf minutes\n", id, elapsed/60);

    ovens += number_of_pizza; // when the order is ready for delivery the ovens are free and ready to bake another order.
    check_signal = pthread_cond_signal(&oven_cond);
    errorCondSignal(check_signal);
    tel++; // when the order is ready for delivery the tel is free and ready to take another order.
    check_signal = pthread_cond_signal(&tel_cond);
    errorCondSignal(check_signal);

    printf("Order with number %d is ready for delivery.\n", id);

    check_unlock = pthread_mutex_unlock(&ready_for_delivery_mtx);
    errorMutex_unlock(check_unlock);

    threadInfo.id = id;
    threadInfo.seed = args.seed;
    deliveryInfo.thread_info= threadInfo;

    return deliveryInfo;
}

void delivery(struct delivery_info deliveryArgs, struct time_measurements timeMeasurements) {
    int id = deliveryArgs.thread_info.id;
    int seed = deliveryArgs.thread_info.seed;
    unsigned int temp_seed = (unsigned int) seed;   // cast seed to unsigned int
    unsigned int *seed_ptr = &temp_seed;            // pointer to seed

    // A mutex where a deliverer delivers an order
    int check_lock = pthread_mutex_lock(&delivery_mtx);
    errorMutex_lock(check_lock);

    while (deliverers == 0) { // if there's no available deliverer wait
        printf("There's no available deliverer for order with number %d. Please wait...\n", id);
        int check_wait =pthread_cond_wait(&delivery_cond, &delivery_mtx);
        errorCondWait(check_wait);
    }
    deliverers--; // if there's an available deliverer take it

    int check_unlock = pthread_mutex_unlock(&delivery_mtx);
    errorMutex_unlock(check_unlock);

    // ride is a random time that the deliverer needs to deliver the order in a range [Tdellow, Tdelhigh]
    int ride = (rand_r(seed_ptr) % (Tdelhigh - Tdellow)) + Tdellow;
    sleep(ride);
    double converted_ride = (double) ride / 60;
    printf("Order with number %d has just delivered in %.2lf minutes\n", id, converted_ride);

    struct timespec finish_y, finish_delivery;
    int check_stop;
    check_stop = clock_gettime(CLOCK_REALTIME, &finish_y);
    errorEndTime(check_stop);

    check_stop = clock_gettime(CLOCK_REALTIME, &finish_delivery);
    errorEndTime(check_stop);

    double elapsed = (finish_y.tv_sec - timeMeasurements.delivery_time_start.tv_sec) +
                     (double) (finish_y.tv_nsec - timeMeasurements.delivery_time_start.tv_nsec) / BILLION;

    double elapsed_cold_order = (finish_delivery.tv_sec - deliveryArgs.t_coldPizza_start.tv_sec) +
                                (double) (finish_delivery.tv_nsec - deliveryArgs.t_coldPizza_start.tv_nsec) / BILLION;

    printf("The order with number %d delivered in %.2lf minutes\n", id, elapsed/60);

    if(max_service_time < elapsed){ // find the max service time
        max_service_time = elapsed;
    }
    sum_service_time += elapsed; // sum all service times

    if(max_cold_time < elapsed_cold_order){ // find the max cold time
        max_cold_time = elapsed_cold_order;
    }
    sum_cold_time += elapsed_cold_order; // sum all cold times
    // The deliverer need the exact same time to return at the store.
    sleep(ride);
    printf("Deliverer from order %d has just returned at the store.\n",id);

    // A mutex where the delivery finishes and the deliverer is ready to undertake another order
    check_lock = pthread_mutex_lock(&end_of_order_mtx);
    errorMutex_lock(check_lock);

    deliverers++;
    int check_signal = pthread_cond_signal(&delivery_cond); // signal that a deliverer is available
    errorCondSignal(check_signal);

    check_unlock = pthread_mutex_unlock(&end_of_order_mtx);
    errorMutex_unlock(check_unlock);
}


void *run_thread(void *args) {
    // A struct where the start of time measurements are stored
    struct time_measurements timeMeasurements;

    // Measure time from the appearance of the customer until the packaging of a successful order
    int check_start_x = clock_gettime(CLOCK_REALTIME, &timeMeasurements.packaging_time_start);
    errorStartTime(check_start_x);

    // Measure time from the appearance of the customer until the delivery of a successful order
    int check_start_y = clock_gettime(CLOCK_REALTIME, &timeMeasurements.delivery_time_start);
    errorStartTime(check_start_y);

    // Stage: Order
    struct order_info order_to_prep = order(args);
    // Stage: Preparation
    struct delivery_info  delivery_args = preparation(order_to_prep, timeMeasurements);
    // Stage: Delivery
    delivery(delivery_args, timeMeasurements);

    free(args); // free memory

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    // Check if the given number of arguments are correct
    if (argc != 3) {
        printf("Wrong number of arguments. The program should take two arguments, the number of customers and seed.\n");
        exit(-1);
    }

    // Convert arguments to integers
    int n_customers = (int) strtol(argv[1], NULL, 10);
    int seed = (int) strtol(argv[2], NULL, 10);

    if (n_customers <= 0) {
        printf("The number of customers should be a positive number.\n");
        exit(-1);
    }

    unsigned int cast_seed = (unsigned int) seed;  // Cast seed to unsigned int
    unsigned int *seed_ptr = &cast_seed; // Pointer to seed

    int next_customer;
    struct thread_arguments *args; // Struct with arguments for each thread

    pthread_t customers[n_customers];  // Array with size n_customers

    // Initialize condition variables
    pthread_cond_init(&cook_cond, NULL);
    pthread_cond_init(&oven_cond, NULL);
    pthread_cond_init(&tel_cond, NULL);
    pthread_cond_init(&delivery_cond, NULL);

    int temp_seed = seed;
    for (int i = 0; i < n_customers; i++) {  // create threads
        args = malloc(sizeof(struct thread_arguments));
        if (args == NULL) {
            printf("NOT ENOUGH MEMORY!\n");
            return -1;
        }
        args->id = i + 1;

        temp_seed += i;
        args->seed = temp_seed;

        int th = pthread_create(&customers[i], NULL, &run_thread, (void *) args);
        errorThread_create(th);

        if (i < n_customers - 1) { // Wait for another order
            next_customer = (rand_r(seed_ptr) % (Torderhigh + 1 - Torderlow)) + Torderlow;
            sleep(next_customer);  // Each next customer connects after a duration specified in the range [Torderlow,Toredrhigh]
        }

    }
    for (int i = 0; i < n_customers; i++) { // join threads
        int th = pthread_join(customers[i], NULL);
        errorThread_join(th);
    }

    // Lock screen when print the results
    int check_lock = pthread_mutex_lock(&print_results_mtx);
    errorMutex_lock(check_lock);

    int total_income = margarita_sales * Cm + peperoni_sales * Cp + special_sales * Cs;
    printf("-----------------------------------------------------\n");
    printf("| Total income: %d euros \n", total_income);
    printf("| Number of margarita pizza: %d \n", margarita_sales);
    printf("| Number of peperoni pizza: %d \n", peperoni_sales);
    printf("| Number of special pizza: %d \n", special_sales);
    int n_successful_orders = n_customers - n_fail_orders;
    printf("| Number of successful orders: %d \n", n_successful_orders);
    printf("| Number of failed orders: %d \n", n_fail_orders);
    printf("| Max service time: %.2lf minutes  \n", max_service_time/60);
    printf("| Average service time: %.2lf minutes \n", (sum_service_time / n_successful_orders)/60);
    printf("| Max cold time: %.2lf minutes   \n", max_cold_time/60);
    printf("| Average cold time: %.2lf minutes \n", (sum_cold_time / n_successful_orders)/60);
    printf("-----------------------------------------------------\n");

    int check_unlock = pthread_mutex_unlock(&print_results_mtx);
    errorMutex_unlock(check_unlock);

    // Destroy mutexes
    pthread_mutex_destroy(&cook_mtx);
    pthread_mutex_destroy(&oven_mtx);
    pthread_mutex_destroy(&tel_mtx);
    pthread_mutex_destroy(&ready_for_delivery_mtx);
    pthread_mutex_destroy(&delivery_mtx);
    pthread_mutex_destroy(&end_of_order_mtx);
    pthread_mutex_destroy(&print_results_mtx);

    // Destroy condition variables
    pthread_cond_destroy(&cook_cond);
    pthread_cond_destroy(&oven_cond);
    pthread_cond_destroy(&tel_cond);
    pthread_cond_destroy(&delivery_cond);

    pthread_exit(NULL);
}