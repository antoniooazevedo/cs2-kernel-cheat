CONFIG_MODULE_SIG=n

obj-m += kernel/cs2.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	g++ user/main.cpp -o out/cheat -lX11

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 
	rm out/cheat
