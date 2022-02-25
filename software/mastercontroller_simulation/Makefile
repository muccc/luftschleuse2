TARGET  := mastercontroller_simulator
SRCS    := mastercontroller_simulator.c \
			cli.c \
			uart-network-bridge.c \
			adc-mock.c \
			io-mock.c \
			lcd-mock.c \
			../mastercontroller/leds.c \
			../mastercontroller/mastercontroller.c \
			../mastercontroller/bus_process.c \
			../mastercontroller/serial_process.c \
			../mastercontroller/command_process.c \
			../mastercontroller/buttons.c \
			../mastercontroller/display_process.c \
			../mastercontroller/serial_handler.c \
			../mastercontroller/lcdhal.c \
			../common/packet.c \
			../common/bus_handler.c \
			../mastercontroller/power_process.c

OBJS    := ${SRCS:.c=.o} 
DEPS    := ${SRCS:.c=.dep} 
XDEPS   := $(wildcard ${DEPS}) 

CFLAGS = -std=gnu99 -O2 -Wall -Werror -ggdb
LDFLAGS =
LIBS    = $(shell pkg-config --libs ncurses)

CFLAGS += -I../mastercontroller -I../common -I.
CFLAGS += -DNATIVE_TARGET
CFLAGS += -DF_CPU=1000000

SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)

CFLAGS += ${SDL_CFLAGS}
LDFLAGS += ${SDL_LDFLAGS} 

.PHONY: all clean distclean 
all:: ${TARGET} 

ifneq (${XDEPS},) 
include ${XDEPS} 
endif 

${TARGET}: ${OBJS} 
	${CC} -o $@ $^ ${LDFLAGS} ${LIBS} 

${OBJS}: %.o: %.c %.dep 
	${CC} ${CFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.c Makefile 
	${CC} ${CFLAGS} -MM $< > $@ 

clean:: 
	-rm -f *~ *.o ${TARGET} ../mastercontroller/*.o ../mastercontroller/*.dep ../common/*.o ../common/*.dep *.dep 

distclean:: clean
