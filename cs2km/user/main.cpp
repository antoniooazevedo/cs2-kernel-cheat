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

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>  

#include "headers/headers.h"
#include "headers/offsets.hpp"
#include "headers/libclient.so.hpp"
#include "headers/buttons.hpp"


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

bool get_key_state(Display* display, KeyCode key)
{
    char keys[32];
    XQueryKeymap(display, keys);

    // Ensure the keycode is within the valid range
    if (key < 8 || key >= 8 * 32) {
        std::cerr << "Keycode out of range" << std::endl;
        return false;
    }

    return keys[key / 8] & (1 << (key % 8));
}   

void read_write_memory(int dev, struct proc_mem* pm, bool write)
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

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <pid>" << std::endl;
        return -1;
    }

    pid_t pid = std::atoi(argv[1]);

    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr)
    {
        std::cerr << "Failed to open X display" << std::endl;
        return -1;
    }



    int dev = open("/dev/comdevice", O_RDONLY);
    if (dev == -1)
    {
        std::cerr << "Failed to open /dev/comdevice: " << strerror(errno) << std::endl;
        //return -1;
    }

    struct proc_mem pm;
    pm.pid = pid;

    const std::string module_name = "libclient.so";

    const uintptr_t base_address = get_module_base_address(pid, module_name);
    if (base_address == 0)
    {
        std::cerr << "Module not found" << std::endl;
        close(dev);
        return -1;
    }

    std::cout << "Base address of " << module_name << ": 0x" << std::hex << base_address << std::endl;
        
    char buffer[64];
    pm.buffer = buffer;

    while (true)
    {

        //get state if END key is pressed
        if (get_key_state(display, XKeysymToKeycode(display, XK_End)))
        {
            std::cout << "END key pressed, exiting..." << std::endl;
            break;
        }
        
        // cheat logic, dump offsets, make bhop, test addresses, etc...
        const uintptr_t localPlayerAddress = base_address + cs2_dumper::offsets::libclient_so::dwLocalPlayerPawn;
        pm.address = localPlayerAddress;
        pm.length = 4; //unsigned long

        read_write_memory(dev, &pm, false);
        const auto localPlayer = *reinterpret_cast<uintptr_t*>(buffer);

        // Get flags
        const auto flagsAddress = localPlayer + cs2_dumper::schemas::libclient_so::C_BaseEntity::m_fFlags;
        pm.address = flagsAddress;
        pm.length = 4; // uint32_t

        read_write_memory(dev, &pm, false);
        const auto flags = *reinterpret_cast<uint32_t*>(buffer);


        // Get individual flags
        const bool in_air = flags & (1 << 0);
        const bool space_pressed = get_key_state(display, XKeysymToKeycode(display, XK_space));

        const auto force_jumpAddress = base_address + cs2_dumper::buttons::jump;
        pm.address = force_jumpAddress;
        pm.length = 4; 
        read_write_memory(dev, &pm, false);
        const auto force_jump = *reinterpret_cast<uint32_t*>(buffer); 

        if (space_pressed && in_air){
            sleep(0.005);

            pm.address = force_jumpAddress;
            uint32_t value = 65537;
            memcpy(pm.buffer, &value, sizeof(value));
            pm.length = 4;

            read_write_memory(dev, &pm, true);
        } else if ((space_pressed && !in_air) || (!space_pressed && force_jump == 65537)){
            pm.address = force_jumpAddress;
            uint32_t value = 256;
            memcpy(pm.buffer, &value, sizeof(value));
            pm.length = 4;

            read_write_memory(dev, &pm, true);
        }
    }

    close(dev);
    return 0;
}

