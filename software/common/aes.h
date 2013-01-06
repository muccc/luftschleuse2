#ifndef AES_H_
#define AES_H_
#include <stdint.h>

void aes_handler_init(void);
void aes_encrypt(uint8_t *block);
void aes_decrypt(uint8_t *block);

#endif
