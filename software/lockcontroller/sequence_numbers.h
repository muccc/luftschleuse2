/*
 *  This file is part of the luftschleuse2 project.
 *
 *  See https://github.com/muccc/luftschleuse2 for more information.
 *
 *  Copyright (C) 2013 Tobias Schneider <schneider@muc.ccc.de> 
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
