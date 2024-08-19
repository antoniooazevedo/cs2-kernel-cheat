savedcmd_/home/work/km/ioctl/ioctl.mod := printf '%s\n'   ioctl.o | awk '!x[$$0]++ { print("/home/work/km/ioctl/"$$0) }' > /home/work/km/ioctl/ioctl.mod
