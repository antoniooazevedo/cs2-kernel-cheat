#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#include "headers.h"

uintptr_t get_module_base_address(pid_t pid, const char* module_name)
{
    char path[256];
    FILE* maps;
    char line[256];
    uintptr_t base_address = 0;

    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    maps = fopen(path, "r");
    if (!maps)
    {
        perror("fopen");
        return 0;
    }

    while (fgets(line, sizeof(line), maps))
    {
        if (strstr(line, module_name))
        {
            sscanf(line, "%lx-%*lx", &base_address);
            break;
        }
    }

    fclose(maps);
    return base_address;
}

// client.dll is libclient.so
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <pid>\n", argv[0]);
        return -1;
    }

    pid_t pid = atoi(argv[1]);

    int dev = open("/dev/comdevice", O_RDONLY);
    if (dev == -1)
    {
        printf("Not successful\n");
        return -1;
    }

    struct proc_mem pm;
    const char* module_name = "libclient.so";

    uintptr_t base_address = get_module_base_address(pid, module_name);
    if (!base_address)
    {
        printf("Module not found\n");
        return -1;
    }
    else
    {
        printf("Base address of %s: 0x%lx\n", module_name, base_address);
    }

    while (1)
    {
        // cheat logic, dump offsets, make bhop, test addresses, etc...
        break;
    }

    close(dev);
    return 0;
}
