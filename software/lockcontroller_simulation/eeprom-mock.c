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
#include <stdint.h>

static uint32_t eeprom_writes = 0;

uint32_t eeprom_read_dword(void *ptr)
{
    return *((uint32_t *)ptr);
}

void eeprom_write_dword(void *ptr, uint32_t v)
{
    *((uint32_t *)ptr) = v;
    eeprom_writes++;
}

uint32_t eeprom_mock_get_write_counter(void)
{
    return eeprom_writes;
}
