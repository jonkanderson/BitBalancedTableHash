/*
Copyright (c) 2020-present Jon K. Anderson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef HASH_BBT_H
#define HASH_BBT_H

#include <stdint.h>

// If you change the bits, be sure to change BBT_HASH_WIDTH in the Makefile.
#define BBT_HASH_WIDTH 64

#if BBT_HASH_WIDTH == 64
   typedef uint64_t bbt_hash_t;
#else
#  define BBT_HASH_WIDTH 32
   typedef uint32_t bbt_hash_t;
#endif

struct bbt_hash_table {
	bbt_hash_t methodMask1;
	bbt_hash_t methodMask2;
	bbt_hash_t size;
	bbt_hash_t *patterns;
};

typedef struct bbt_hash_ctxt {
	struct bbt_hash_table *table;
	bbt_hash_t pos;
	bbt_hash_t hash;
} bbt_hash_ctxt;

#define BBT_HASH_GET(C) ((C)->hash)

void bbt_hash_init(bbt_hash_ctxt *ctxt, struct bbt_hash_table *ht);
void bbt_hash_reset(bbt_hash_ctxt *ctxt);
void bbt_hash_add(bbt_hash_ctxt *ctxt, unsigned char *input, unsigned input_sz);

#endif

