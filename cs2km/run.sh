#! /bin/bash

# Compile the user program and the kernel module
make

# Load kernel module
sudo insmod ./kernel/cs2.ko

# Run the user program
./out/cheat $(pidof cs2) 

# Unload kernel module
sudo rmmod cs2

# Clean up
echo "yes" | make clean
