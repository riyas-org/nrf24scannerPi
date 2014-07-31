# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Wall -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
#CCFLAGS=

# define all programs
#PROGRAMS = scanner pingtest pongtest
PROGRAMS = scanner 
SOURCES = ${PROGRAMS:=.cpp} gpio.o spi.o compatibility.o

#all: ${PROGRAMS}
all: scanner.o gpio.o spi.o compatibility.o
	g++  ${CCFLAGS} scanner.o -o  scanner compatibility.o gpio.o spi.o 

${PROGRAMS}: ${SOURCES}
#	g++ ${CCFLAGS} -Wall -L../librf24/  -lrf24 $@.cpp -o $@
	g++ ${CCFLAGS} $@.cpp -o $@

gpio.o: gpio.cpp
	g++ -Wall -fPIC ${CCFLAGS} -c gpio.cpp

spi.o: spi.cpp
	g++ -Wall -fPIC ${CCFLAGS} -c spi.cpp

compatibility.o: compatibility.c
	gcc -Wall -fPIC  ${CCFLAGS} -c compatibility.c

scanner.o: scanner.cpp
	gcc -Wall -fPIC  ${CCFLAGS} -c scanner.cpp

