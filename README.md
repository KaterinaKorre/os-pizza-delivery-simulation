# OS Pizza Delivery Simulation (POSIX Threads)

This project simulates a pizza ordering and delivery system using POSIX threads (pthreads).  
It was developed as part of the Operating Systems course at the Athens University of Economics and Business.

## Features
- Concurrent order processing: each customer handled by a separate thread
- Resource management: telephone operators, cooks, ovens, and deliverers
- Synchronization using mutexes and condition variables
- Randomized events: order size, pizza types, payment success/failure, delivery times
- Statistics & reporting:
  - Total income
  - Pizzas sold per type
  - Successful vs failed orders
  - Average & maximum service time
  - Average & maximum cooling time

## Tech Stack
- Language: C
- Threads: POSIX (pthreads)
- Build Tool: GCC with `-pthread`
- Error handling: custom utility functions (`utils.c/.h`)
- Timing: `clock_gettime()`, `sleep()`

## How to Build & Run

### 1. Compile manually
- gcc src/p3210084-pizza.c src/utils.c -o run -pthread
- ./run <Ncust> <Seed>

- Ncust: number of customers (threads to create)
- Seed: random seed for reproducibility

# Example:
./run 100 10

### 2. Run via helper script
- chmod +x scripts/test-res.sh
- ./scripts/test-res.sh
- This compiles and runs with 100 customers and seed 10.

## Project Structure
src/
- pizza.c                # main simulation logic
- pizza.h                # constants, structs, function prototypes
- utils.c / utils.h      # error-handling helpers
scripts/
- test-res.sh            # compile & run script

## Sample Output
- The order with number 3 was registered.
- The order with number 3 has finished the preparation in 12.00 minutes
- The order with number 3 delivered in 18.00 minutes
- | Total income: 450 euros
- | Number of margarita pizza: 12
- | Number of peperoni pizza: 9
- | Number of special pizza: 14
- | Number of successful orders: 29
- | Number of failed orders: 1
- | Max service time: 22.00 minutes
- | Average service time: 15.30 minutes
- | Max cold time: 6.00 minutes
- | Average cold time: 3.10 minutes

## What I Learned

- Designing multi-threaded programs with shared resources
- Using mutex locks and condition variables correctly
- Handling randomized events in simulations
- Collecting and reporting performance metrics
- Writing clean, modular C code for a real-world workflow simulation

