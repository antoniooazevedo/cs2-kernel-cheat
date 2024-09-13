#ifndef MEMORY_H
#define MEMORY_H

#include <sys/ioctl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <string.h>
#include <fcntl.h>

#include "../headers/headers.h"

static int dev = open("/dev/comdevice", O_RDONLY);

void read_write_memory(struct proc_mem* pm, bool write)
{
    if (write){
        ioctl(dev, WR_PROC_MEM, pm);
    } else {
        ioctl(dev, RD_PROC_MEM, pm);

        if (pm->buffer == 0){
            std::cerr << "Failed to read address 0x" << std::hex << pm->address << std::endl;
        }
    }
}

template <typename T>
T read_memory_from_address(uintptr_t address, struct proc_mem& pm, std::size_t length = 0)
{
    pm.address = address;
    pm.length = length == 0 ? sizeof(T) : length;

    read_write_memory(&pm, false);

    return *reinterpret_cast<T*>(pm.buffer);
}
template <typename V>
void write_memory_to_address(uintptr_t address, V value, struct proc_mem& pm)
{
    pm.address = address;
    pm.length = sizeof(V);
    memcpy(pm.buffer, &value, sizeof(V));

    read_write_memory(&pm, true);
}

uintptr_t get_module_base_address(pid_t pid, const std::string& module_name)
{
    std::string path = "/proc/" + std::to_string(pid) + "/maps";
    std::ifstream maps(path);
    std::string line;
    uintptr_t base_address = 0;

    if (!maps.is_open())
    {
        std::cerr << "Error opening file: " << strerror(errno) << std::endl;
        return 0;
    }

    while (std::getline(maps, line))
    {
        if (line.find(module_name) != std::string::npos)
        {
            std::istringstream iss(line);
            iss >> std::hex >> base_address;
            break;
        }
    }

    return base_address;
}

#endif 