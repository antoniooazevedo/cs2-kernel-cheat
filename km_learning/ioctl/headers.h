#ifndef HEADERS_H
#define HEADERS_H

struct greeter_struct{
	int repeat;
	char name[64];
};

#define WR_VALUE _IOW('a', 'b', int32_t *)
#define RD_VALUE _IOR('a', 'b', int32_t *)
#define GREETER _IOR('a', 'b', struct greeter_struct)

#endif
