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
#include "sequence_numbers.h"

#include <avr/eeprom.h>

#include <stdbool.h>
#include <stdint.h>

static uint32_t sequence_numbers_rx_ee[SEQUENCE_NUMBERS_SLOTS] EEMEM;

static uint32_t sequence_numbers_tx;
static uint32_t sequence_numbers_rx;
static uint32_t sequence_numbers_rx_persisted;

static uint32_t sequence_numbers_get_max(uint32_t *array_ee);
static void sequence_numbers_write_next(uint32_t *array_ee, uint32_t next);
static void sequence_numbers_check_next(uint32_t *array_ee, uint32_t next);

void sequence_numbers_init(void)
{

    sequence_numbers_rx_persisted =
            sequence_numbers_get_max(sequence_numbers_rx_ee);
    sequence_numbers_rx = sequence_numbers_rx_persisted;
    sequence_numbers_tx = 0;
}

uint32_t sequence_numbers_get_tx(void)
{
    return sequence_numbers_tx;
}

void sequence_numbers_increment_tx(void)
{
    sequence_numbers_tx++;
}

void sequence_numbers_set_tx(uint32_t tx_seq)
{
    sequence_numbers_tx = tx_seq;
}

bool sequence_numbers_check_rx(uint32_t seq)
{
    bool result = false;
    if( seq >= sequence_numbers_rx ){
        result = true;
        sequence_numbers_check_next(sequence_numbers_rx_ee,
            sequence_numbers_rx);
        sequence_numbers_rx = seq;
    }
    return result;
}

uint32_t sequence_numbers_get_expected_rx(void)
{
    return sequence_numbers_rx;
}

uint32_t sequence_numbers_get_persisted_rx(void)
{
    
    return sequence_numbers_rx_persisted;
}
/*
 * \brief
 *      Checks if it is necessary to write a new
 *      incremented value to the EEPROM.
 *
 *  If the sequence number reaches or surpasses
 *  the persisted sequence number, the sequence
 *  number + a full increment will be written to
 *  the next free location inside the EEPROM.
 */
static void sequence_numbers_check_next(uint32_t *array_ee,
                                        uint32_t next)
{
    if( next >= sequence_numbers_rx_persisted ){
        sequence_numbers_write_next(array_ee,
                next + SEQUENCE_NUMBERS_LEAP);
        sequence_numbers_rx_persisted =
            sequence_numbers_get_max(sequence_numbers_rx_ee);
    }
}

/*
 * \brief
 *      Returns the highest sequence number stored inside an
 *      array inside the EEPROM.
 */
static uint32_t sequence_numbers_get_max(uint32_t *array_ee)
{
    uint16_t i;
    uint32_t max = 0;
    for( i = 0; i < SEQUENCE_NUMBERS_SLOTS; i++ ){
        uint32_t u = eeprom_read_dword(array_ee+i);
        if( u > max ){
            max = u;
        }
    }

    return max;
}

static void sequence_numbers_write_next(uint32_t *array_ee, uint32_t next)
{
    uint16_t i;
    uint32_t max = 0;
    uint16_t max_i = 0;
    for( i = 0; i < SEQUENCE_NUMBERS_SLOTS; i++ ){
        uint32_t u = eeprom_read_dword(array_ee+i);
        if( u > max ){
            max = u;
            max_i = i;
        }
    }
    max_i = max_i+1;
    if( max_i == SEQUENCE_NUMBERS_SLOTS ){
        max_i = 0;
    }
    eeprom_write_dword(array_ee+max_i, next);
}

