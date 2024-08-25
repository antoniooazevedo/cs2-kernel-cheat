#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cerrno>
#include <string.h>

#include "headers.h"

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

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <pid>" << std::endl;
        return -1;
    }

    pid_t pid = std::atoi(argv[1]);

    int dev = open("/dev/comdevice", O_RDONLY);
    if (dev == -1)
    {
        std::cerr << "Failed to open /dev/comdevice: " << strerror(errno) << std::endl;
        //return -1;
    }

    struct proc_mem pm;
    const std::string module_name = "libclient.so";

    uintptr_t base_address = get_module_base_address(pid, module_name);
    if (base_address == 0)
    {
        std::cerr << "Module not found" << std::endl;
        close(dev);
        return -1;
    }
    else
    {
        std::cout << "Base address of " << module_name << ": 0x" << std::hex << base_address << std::endl;
    }

    while (true)
    {
        // cheat logic, dump offsets, make bhop, test addresses, etc...
        break;
    }

    close(dev);
    return 0;
}

