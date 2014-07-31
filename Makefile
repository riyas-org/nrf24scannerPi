# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s

all: scanner.o bcm2835.o
	g++  ${CCFLAGS} scanner.o -o  scanner bcm2835.o 

bcm2835.o: bcm2835.c
	gcc -Wall -fPIC ${CCFLAGS} -c bcm2835.c

scanner.o: scanner.cpp
	gcc -Wall -fPIC  ${CCFLAGS} -c scanner.cpp

