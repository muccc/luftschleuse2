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
