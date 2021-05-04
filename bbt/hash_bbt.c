/*
Copyright (c) 2020-present Jon K. Anderson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "hash_bbt.h"

#include <string.h>

void bbt_hash_init(bbt_hash_ctxt *ctxt, struct bbt_hash_patterns *patterns) {
	ctxt->patterns = patterns;
	ctxt->inputSize = 0;
	ctxt->commandA.accum = patterns->commandPatterns[0];
	ctxt->commandA.pos = 0;
	ctxt->commandB.accum = patterns->commandPatterns[1];
	ctxt->commandB.pos = 0;
	ctxt->hash.accum = 0;
	ctxt->hash.pos = 0;
}

void bbt_hash_reset(bbt_hash_ctxt *ctxt) {
	struct bbt_hash_patterns *patterns = ctxt->patterns;
	ctxt->inputSize = 0;
	ctxt->commandA.accum = patterns->commandPatterns[0];
	ctxt->commandA.pos = 0;
	ctxt->commandB.accum = patterns->commandPatterns[1];
	ctxt->commandB.pos = 0;
	ctxt->hash.accum = 0;
	ctxt->hash.pos = 0;
}

// BIT_ROTATE moves ROT bits from the high end to low.
#define BIT_ROTATE(HASH,ROT) ((bbt_hash_t)((HASH)<<(ROT)) | (bbt_hash_t)((HASH)>>(BBT_HASH_WIDTH - (ROT))))

void bbt_hash_cmdIterate(
	bbt_hash_t *patArray, unsigned patSz,
	struct bbt_hash_ctxt_gen *gen, unsigned char input
) {
	unsigned int cmdRot, cmdAdvance;
	bbt_hash_t buffer;
	// Get cmdRot and cmdAdvance.  Using the nibble of input.
	buffer = gen->accum ^ input;
	cmdRot = (buffer & 0x1F);
	buffer = (gen->accum >> 6) ^ input;
	cmdAdvance = (buffer & 0x0F) + 1;
	gen->accum = BIT_ROTATE(gen->accum, 10);

	buffer= patArray[gen->pos];
	if (cmdRot > 0) {
		buffer = BIT_ROTATE(buffer, cmdRot);
	}
	gen->accum ^= buffer;
	gen->pos = (gen->pos + cmdAdvance) % patSz;
}

void bbt_hash_calc(bbt_hash_ctxt *ctxt, unsigned char *input, unsigned input_sz) {
	unsigned int hashRot, hashAdvance;
	bbt_hash_t buffer;
	struct bbt_hash_patterns *patterns = ctxt->patterns;

	// Special case.  The empty string gets a hash value of 0.
	if (input_sz == 0) return;

	ctxt->inputSize += input_sz;

	unsigned char *inputPtr = input;
	for (unsigned i=0; i<input_sz; i++, inputPtr++) {
		// Input only influences the way that command accumulators change.
		bbt_hash_t inByte = *inputPtr;

		bbt_hash_cmdIterate(
			patterns->commandPatterns, patterns->commandPatternsSize,
			&ctxt->commandA, inByte & 0x0F);

		inByte = inByte >> 4;

		bbt_hash_cmdIterate(
			patterns->commandPatterns, patterns->commandPatternsSize,
			&ctxt->commandB, inByte & 0x0F);

		// Get hashRot and hashAdvance from the current command accumulators.
		buffer = ctxt->commandA.accum ^ ctxt->commandB.accum;
		hashRot = buffer & 0x1F;
		hashAdvance = ((buffer >> 5) & 0xFF) + 1;
		ctxt->commandA.accum = BIT_ROTATE(ctxt->commandA.accum, 8);
		ctxt->commandB.accum = BIT_ROTATE(ctxt->commandB.accum, 8);

		// Calculate the next value on the hash using the pattern.
		ctxt->hash.pos = (ctxt->hash.pos + hashAdvance) % patterns->hashPatternsSize;
		buffer = patterns->hashPatterns[ctxt->hash.pos];
		if (hashRot > 0) {
			buffer = BIT_ROTATE(buffer, hashRot);
		}
		ctxt->hash.accum ^= buffer;
	}
}

bbt_hash_t bbt_hash_getHash(bbt_hash_ctxt *ctxt) {
	if (ctxt->inputSize == 0) { return 0; }
	bbt_hash_t retValue;
	bbt_hash_ctxt tempCtxt;
	memcpy(&tempCtxt, ctxt, sizeof(bbt_hash_ctxt));
	bbt_hash_calc(&tempCtxt, (unsigned char[sizeof(bbt_hash_ctxt)]){}, sizeof(bbt_hash_ctxt));
	return BBT_HASH_GET(&tempCtxt);
}

