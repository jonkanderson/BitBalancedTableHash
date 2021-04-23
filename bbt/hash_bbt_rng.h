/*
Copyright (c) 2020-present Jon K. Anderson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef HASH_BBT_RNG_H
#define HASH_BBT_RNG_H

#include "hash_bbt.h"

typedef struct bbt_rng_ctxt {
	bbt_hash_ctxt ctxtA;
	bbt_hash_ctxt ctxtB;
} bbt_rng_ctxt;

void bbt_rng_init(bbt_rng_ctxt *rng, struct bbt_hash_params *paramsA, struct bbt_hash_params *paramsB, bbt_hash_t initialHash);
void bbt_rng_phrase(bbt_rng_ctxt *rng, unsigned char *input, unsigned input_sz);
bbt_hash_t bbt_rng_next(bbt_rng_ctxt *rng);

#endif

