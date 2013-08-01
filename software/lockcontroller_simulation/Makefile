TARGET  := lockcontroller_simulator
SRCS    :=  ../lockcontroller_simulation/lockcontroller_simulator.c \
			../lockcontroller_simulation/cli.c \
			../lockcontroller_simulation/io-mock.c \
			../mastercontroller_simulation/uart-network-bridge.c \
			../lockcontroller_simulation/adc-mock.c \
			../lockcontroller_simulation/aes/aes.c \
			../lockcontroller_simulation/eeprom-mock.c \
			../lockcontroller/leds.c \
			../lockcontroller/lockcontroller.c \
			../lockcontroller/bus_process.c \
			../lockcontroller/command_process.c \
			../lockcontroller/buttons.c \
			../lockcontroller/door-2.c \
			../lockcontroller/door-1.c \
			../lockcontroller_simulation/door-mock.c \
			../lockcontroller/bell_process.c \
			../lockcontroller/sequence_numbers.c \
			../common/packet.c \
			../common/aes.c \
			../common/bus_handler.c \
			../lockcontroller/power_process.c

OBJS    := ${SRCS:.c=.o} 
DEPS    := ${SRCS:.c=.dep} 
XDEPS   := $(wildcard ${DEPS}) 

CFLAGS = -std=gnu99 -O2 -Wall -Werror -ggdb 
LDFLAGS = 
LIBS    = -lcurses -lcrypto -lssl

CFLAGS += -I../lockcontroller -I../common -I../lockcontroller_simulation -I.
CFLAGS += -DNATIVE_TARGET
#CFLAGS += -DDEBUG
CFLAGS += -DF_CPU=1000000

.PHONY: all clean distclean 
all:: ${TARGET} 

ifneq (${XDEPS},) 
include ${XDEPS} 
endif 

${TARGET}: ${OBJS} 
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.c %.dep 
	${CC} ${CFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.c Makefile 
	${CC} ${CFLAGS} -MM $< > $@ 

clean:: 
	-rm -f *~ *.o ${TARGET} *.dep 
	-rm -f ../lockcontroller/*.o ../lockcontroller/*.dep
	-rm -f ../common/*.o ../common/*.dep
	-rm -f ../mastercontroller_simulation/*.o ../mastercontroller_simulation/*.dep
	-rm -f ../lockcontroller_simulation/*.o ../lockcontroller_simulation/*.dep

distclean:: clean
