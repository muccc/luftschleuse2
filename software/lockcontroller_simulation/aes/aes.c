#include "aes.h"

#include <openssl/aes.h>
#include <string.h>
#include <stdio.h>

void aes128_init(const void* key, aes128_ctx_t* ctx)
{
    AES_set_encrypt_key(key, 128, &(ctx->enc_key));
    AES_set_decrypt_key(key, 128,&(ctx->dec_key));
}

void aes128_enc(void* buffer, aes128_ctx_t* ctx)
{
    uint8_t enc_out[16];
    AES_encrypt(buffer, enc_out, &(ctx->enc_key));
    memcpy(buffer, enc_out, 16);
}

void aes128_dec(void* buffer, aes128_ctx_t* ctx)
{
    uint8_t dec_out[16];
    AES_decrypt(buffer, dec_out, &(ctx->dec_key));
    memcpy(buffer, dec_out, 16);
}

