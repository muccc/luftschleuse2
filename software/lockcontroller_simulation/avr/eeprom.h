#ifndef AVR_EEPROM_H
#define AVR_EEPROM_H

#define EEMEM
uint32_t eeprom_read_dword(void *ptr);
void eeprom_write_dword(void *ptr, uint32_t v);

#endif

