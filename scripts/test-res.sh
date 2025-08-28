#!/bin/bash

# Compile the C program
gcc pizza.c utils.c -o run -pthread

# Run the compiled program with arguments
./run 100 10