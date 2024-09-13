#! /bin/bash

# Compile the user program and the kernel module
make

# Setup device file
if [ ! -e /dev/comdevice ]; then
    mknod /dev/comdevice c 90 0
fi

# Load kernel module
sudo insmod ./kernel/cs2.ko

# Run the user program
./out/cheat $(pidof cs2) 

# Unload kernel module
sudo rmmod cs2

# Clean up
echo "yes" | make clean