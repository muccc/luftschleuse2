#ifndef AES_H_
#define AES_H_

#include <openssl/aes.h>
#include <stdint.h>

typedef struct{
    AES_KEY enc_key;
    AES_KEY dec_key;
} aes128_ctx_t;


void aes128_init(const void* key, aes128_ctx_t* ctx);
void aes128_enc(void* buffer, aes128_ctx_t* ctx);
void aes128_dec(void* buffer, aes128_ctx_t* ctx);
#endif
