# Operating Systems – Pizza Delivery Simulation

This project simulates a pizza delivery system using POSIX threads and is developed as part of the Operating Systems course at AUEB.

## Course Info

- Course: Operating Systems
- Academic Year: 2024–2025

## Project Description

The simulation models a real-world pizza ordering and delivery workflow. It includes:

- Order reception via telephone operators
- Pizza preparation by cooks
- Baking in ovens
- Final delivery by delivery personnel

All entities are synchronized using mutexes and condition variables to simulate concurrent access and contention over shared resources.

## Implementation

### Technologies:
- Language: C
- Threads: POSIX (pthreads)
- Timing: `clock_gettime()` for nanosecond precision
- Compilation: `gcc -pthread`

### Key Concepts:
- Thread creation & joining
- Mutex locking/unlocking
- Condition signaling & waiting
- Time measurement for performance analysis
- Error handling via utility wrappers

## How to Build and Run

### Option 1 – Run script

chmod +x scripts/test-res.sh
./scripts/test-res.sh

This compiles the source code and runs the simulation with:

100 orders

10-second time limit

### Option 2 – Manual compilation

gcc src/p3210084-pizza.c src/utils.c -o run -pthread
./run 100 10




