/*
Copyright (c) 2020-present Jon K. Anderson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "hash_bbt_rng.h"

void bbt_rng_init(bbt_rng_ctxt *rng, struct bbt_hash_params *paramsA, struct bbt_hash_params *paramsB, bbt_hash_t initialHash) {
	bbt_hash_init(&rng->ctxtA, paramsA);
	bbt_hash_init(&rng->ctxtB, paramsB);
	rng->ctxtA.hash = initialHash;
}

void bbt_rng_phrase(bbt_rng_ctxt *rng, unsigned char *input, unsigned input_sz){
	bbt_hash_calc(&rng->ctxtA, input, input_sz);
}

// The function bbtrng_getNext is only called by bbt_rng_next.
bbt_hash_t bbtrng_getNext(bbt_hash_ctxt *ctxtThis, bbt_hash_ctxt *ctxtOther) {
	bbt_hash_t num = BBT_HASH_GET(ctxtThis);
	unsigned char outByte = num & 0xFF;
	num = num >> 8;

	unsigned char buffer[sizeof(bbt_hash_t)];
	unsigned char *p = buffer;
	for (unsigned i=0; i < sizeof(bbt_hash_t)-1; i++, p++) {
		*p = num & 0xFF;
		num = num >> 8;
	}
	bbt_hash_calc(ctxtOther, buffer, sizeof(bbt_hash_t)-1);
	return BBT_HASH_GET(ctxtOther);
}

bbt_hash_t bbt_rng_next(bbt_rng_ctxt *rng) {
	bbtrng_getNext(&rng->ctxtA, &rng->ctxtB);
	return bbtrng_getNext(&rng->ctxtB, &rng->ctxtA);
}

