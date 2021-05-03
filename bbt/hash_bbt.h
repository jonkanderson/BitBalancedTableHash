/*
Copyright (c) 2020-present Jon K. Anderson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef HASH_BBT_H
#define HASH_BBT_H

#include <stdint.h>

#ifndef BBT_HASH_WIDTH
#define BBT_HASH_WIDTH 64
#endif

#if BBT_HASH_WIDTH == 64
   typedef uint64_t bbt_hash_t;
#elif BBT_HASH_WIDTH == 32
   typedef uint32_t bbt_hash_t;
#else
#error "BBT_HASH_WIDTH is not defined."
#endif

typedef struct bbt_hash_patterns {
	unsigned hashPatternsSize;
	bbt_hash_t *hashPatterns;
	unsigned commandPatternsSize;
	bbt_hash_t *commandPatterns;
} bbt_hash_patterns;

struct bbt_hash_ctxt_gen {
	bbt_hash_t accum;
	unsigned pos;
};

typedef struct bbt_hash_ctxt {
	struct bbt_hash_patterns *patterns;
	struct bbt_hash_ctxt_gen command;
	struct bbt_hash_ctxt_gen hash;
	unsigned inputSize;
} bbt_hash_ctxt;

/*
 * Usage notes:
 *   bbt_hash_init = Initializes a hash context that has been pre-allocated.
 *   bbt_hash_reset = Re-initializes a hash context using the same patterns.
 *   bbt_hash_calc = Consumes input into the hash.  This can be called multiple times
 *      to add more input into the hash.
 *   BBT_HASH_GET = Macro to get the current hash accumulator value.  This
 *      does not include the full effects of the most recent input bytes added.
 *   bbt_hash_getHash = Pushes zeros into a copy of the machine to utilize the
 *      rest of the input.  Original state is unchanged. Also, as a special case,
 *      the empty string always returns a hash value of 0.
 */

#define BBT_HASH_GET(C) ((C)->hash.accum)

void bbt_hash_init(bbt_hash_ctxt *ctxt, struct bbt_hash_patterns *patterns);
void bbt_hash_reset(bbt_hash_ctxt *ctxt);
void bbt_hash_calc(bbt_hash_ctxt *ctxt, unsigned char *input, unsigned input_sz);
bbt_hash_t bbt_hash_getHash(bbt_hash_ctxt *ctxt);

#endif

