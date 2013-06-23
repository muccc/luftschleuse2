#include "aes/aes.h"
#include <avr/io.h>
#include <stdint.h>
#if 0
static const uint8_t aes_key[16]  = { 0x0, 0x0, 0x0, 0x0,
                   0x0, 0x0, 0x0, 0x0,
                   0x0, 0x0, 0x0, 0x0,
                   0x0, 0x0, 0x0, 0x0 };
#endif

#include "door-config.h"
static const uint8_t aes_key[16]  = {AES_KEY};

static aes128_ctx_t aes_ctx; /* the context where the round keys are stored */

void aes_handler_init(void)
{
    aes128_init(aes_key, &aes_ctx); /* generating the round keys from the 128 bit key */
}

void aes_encrypt(uint8_t *block)
{
    aes128_enc(block, &aes_ctx);
}

void aes_decrypt(uint8_t *block)
{
    aes128_dec(block, &aes_ctx);
}

