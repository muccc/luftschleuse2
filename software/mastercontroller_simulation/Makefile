TARGET  := mastercontroller_simulator
SRCS    := mastercontroller_simulator.c \
			cli.c \
			uart-network-bridge.c \
			adc-mock.c \
			io-mock.c \
			../mastercontroller/leds.c \
			../mastercontroller/mastercontroller.c \
			../mastercontroller/bus_process.c \
			../mastercontroller/serial_process.c \
			../mastercontroller/command_process.c \
			../mastercontroller/buttons.c \
			../mastercontroller/display_process.c \
			../mastercontroller/serial_handler.c \
			../mastercontroller/lcd.c \
			../mastercontroller/lcdhal.c \
			../common/packet.c \
			../common/bus_handler.c \
			../mastercontroller/power_process.c

OBJS    := ${SRCS:.c=.o} 
DEPS    := ${SRCS:.c=.dep} 
XDEPS   := $(wildcard ${DEPS}) 

CFLAGS = -std=gnu99 -O2 -Wall -Werror -ggdb 
LDFLAGS = 
LIBS    = -lcurses 

CFLAGS += -I../mastercontroller -I../common -I.
CFLAGS += -DNATIVE_TARGET
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
	-rm -f *~ *.o ${TARGET} ../mastercontroller/*.o ../mastercontroller/*.dep ../common/*.o ../common/*.dep *.dep 

distclean:: clean
