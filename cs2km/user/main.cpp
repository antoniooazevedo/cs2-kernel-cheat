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


#include "headers/headers.h"
#include "headers/offsets.hpp"
#include "headers/libclient.so.hpp"
#include "headers/buttons.hpp"
#include "headers/keyboard.hpp"
#include "headers/memory.hpp"

#include "hacks/bhop.hpp"
#include "hacks/radar.hpp"


int main(int argc, char* argv[])
{
    //! GETTING PID OF CS2
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <pid>" << std::endl;
        return -1;
    }
    pid_t pid = std::atoi(argv[1]);

    //! INITIALIZING PROC_MEM STRUCT
    struct proc_mem pm;
    char buffer[64];
    pm.pid = pid;
    pm.buffer = buffer;


    //! GETTING BASE ADDRESS OF libclient.so
    const std::string module_name = "libclient.so";
    const uintptr_t base_address = get_module_base_address(pid, module_name);
    if (base_address == 0)
    {
        std::cerr << "Module not found" << std::endl;
        close(dev);
        return -1;
    }
        
    //! MAIN LOOP
    while (true)
    {
        //get state if END key is pressed
        if (get_key_state(XKeysymToKeycode(display, XK_End)))
        {
            std::cout << "END key pressed, exiting..." << std::endl;
            break;
        }
        
        // cheat logic, dump offsets, make bhop, test addresses, etc...
        const uintptr_t localPlayerAddress = base_address + cs2_dumper::offsets::libclient_so::dwLocalPlayerPawn;
        pm.address = localPlayerAddress;
        pm.length = 4; //unsigned long
        read_write_memory(&pm, false);
        const auto localPlayer = *reinterpret_cast<uintptr_t*>(buffer);

        // Run hacks
        if (!bhop(base_address, localPlayer, pm)) sleep(0.005);
        else if (!radar(base_address, localPlayer, pm)) sleep(0.005);

    }

    close(dev);
    return 0;
}

