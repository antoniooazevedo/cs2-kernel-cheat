#ifndef HEADERS_H
#define HEADERS_H

struct greeter_struct{
	int repeat;
	char name[64];
};

struct proc_mem {
    pid_t pid;                 // Process ID to which memory belongs
    uintptr_t address;             // Address in the process's memory
    void *buffer;              // Buffer to read/write data
    size_t length;             // Length of data to read/write
};

#define WR_VALUE _IOW('a', 'b', int32_t *)
#define RD_VALUE _IOR('a', 'b', int32_t *)
#define WR_PROC_MEM _IOW('a', 'a', struct proc_mem)
#define RD_PROC_MEM _IOR('a', 'b', struct proc_mem)
#define GREETER _IOR('a', 'b', struct greeter_struct)

#endif
