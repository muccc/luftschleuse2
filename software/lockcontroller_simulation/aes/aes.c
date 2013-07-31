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

