#ifndef SEQUENCE_NUMBERS_H
#define SEQUENCE_NUMBERS_H

#include <stdint.h>
#include <stdbool.h>

void sequence_numbers_init(void);
uint32_t sequence_numbers_get_tx(void);
void sequence_numbers_set_tx(uint32_t tx_seq);
bool sequence_numbers_check_rx(uint32_t seq);
uint32_t sequence_numbers_get_expected_rx(void);

/* After n seq numbers a new entry gets written
 * to the eeprom.
 */
#define SEQUENCE_NUMBERS_LEAP   (1024UL)

/*
 * Amount of slots for the seq number round robin.
 * Every entry takes 4 Bytes. There are entries for
 * rx and tx.
 *
 * The ATMega664P has 4k Bytes EEPROM.
 * 512 * 4 Bytes * 2 = 4k Bytes
 */
#define SEQUENCE_NUMBERS_SLOTS  512


#endif
