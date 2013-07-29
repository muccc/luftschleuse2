#ifndef __PINUTILS_H_
#define __PINUTILS_H_

#include <avr/io.h>

#ifndef NATIVE_TARGET

#define _PORT_CHAR(character) PORT ## character
#define PORT_CHAR(character) _PORT_CHAR(character)

#define _PIN_CHAR(character) PIN ## character
#define PIN_CHAR(character) _PIN_CHAR(character)
 
#define _DDR_CHAR(character) DDR ## character
#define DDR_CHAR(character) _DDR_CHAR(character)


#define DDR_CONFIG_IN(pin)  DDR_CHAR( pin ## _PORT) &= ~_BV((pin ## _PIN))
#define DDR_CONFIG_OUT(pin)  DDR_CHAR( pin ## _PORT) |= _BV((pin ## _PIN))

#define PIN_BV(pin) (_BV(pin ## _PIN))
#define PIN_NR(pin) (pin ## _PIN)
#define PIN_HIGH(pin) (PIN_CHAR(pin ## _PORT) & _BV(pin ## _PIN))
#define PIN_SET(pin) PORT_CHAR(pin ## _PORT) |= _BV(pin ## _PIN)
#define PIN_CLEAR(pin) PORT_CHAR(pin ## _PORT) &= ~_BV(pin ## _PIN)
#define PIN_TOGGLE(pin) PORT_CHAR(pin ## _PORT) ^= _BV(pin ## _PIN)
#define PIN_PULSE(pin) do { PORT_CHAR(pin ## _PORT) &= ~_BV(pin ## _PIN); \
                              PORT_CHAR(pin ## _PORT) ^= _BV(pin ## _PIN); } while(0)

#else /* NATIVE_TARGET */
#include "io-mock.h"

#define DDR_CONFIG_IN(pin) io_ddr_config_in(#pin)
#define DDR_CONFIG_OUT(pin) io_ddr_config_out(#pin)
#define PIN_SET(pin) io_pin_set(#pin)
#define PIN_CLEAR(pin) io_pin_clear(#pin)
#define PIN_TOGGLE(pin) io_pin_toggle(#pin)
#define PIN_PULSE(pin) io_pin_pulse(#pin)
#define PIN_HIGH(pin) io_pin_high_(#pin)
#endif /* NATIVE_TARGET */
#endif
