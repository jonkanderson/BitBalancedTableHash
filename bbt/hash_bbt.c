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
	ctxt->hashAccum = 0;
	ctxt->commandAccum = patterns->commandPatterns[0];
	ctxt->inputSize = 0;
	ctxt->hashPatternsPos = 0;
	ctxt->commandPatternsPos = 0;
}

void bbt_hash_reset(bbt_hash_ctxt *ctxt) {
	ctxt->hashAccum = 0;
	ctxt->commandAccum = ctxt->patterns->commandPatterns[0];
	ctxt->inputSize = 0;
	ctxt->hashPatternsPos = 0;
	ctxt->commandPatternsPos = 0;
}

// BIT_ROTATE moves ROT bits from the high end to low.
#define BIT_ROTATE(HASH,ROT) ((bbt_hash_t)((HASH)<<(ROT)) | (bbt_hash_t)((HASH)>>(BBT_HASH_WIDTH - (ROT))))

void bbt_hash_calc(bbt_hash_ctxt *ctxt, unsigned char *input, unsigned input_sz) {
	unsigned int cmdRot, cmdAdvance;
	unsigned int hashRot, hashAdvance;
	bbt_hash_t buffer;
	struct bbt_hash_patterns *patterns = ctxt->patterns;

	// Special case.  No input gets a hash value of 0.
	if (input_sz == 0) return;

	ctxt->inputSize += input_sz;

	unsigned char *inputPtr = input;
	for (unsigned i=0; i<input_sz; i++, inputPtr++) {
		// Input only influences the way that commandAccum changes.
		bbt_hash_t inByte = *inputPtr;

		// Get cmdRot and cmdAdvance.  Using the first nibble of the input.
		buffer = ctxt->commandAccum ^ (inByte & 0x0F);
		cmdRot = (buffer & 0x1F);
		buffer = (ctxt->commandAccum >> 6) ^ ((bbt_hash_t)*inputPtr);
		cmdAdvance = (buffer & 0xFF) + 1;
		ctxt->commandAccum = BIT_ROTATE(ctxt->commandAccum, 7);

		// Apply first iteration to command generator.
		buffer= patterns->commandPatterns[ctxt->commandPatternsPos];
		if (cmdRot > 0) {
			buffer = BIT_ROTATE(buffer, cmdRot);
		}
		ctxt->commandAccum ^= buffer;
		ctxt->commandPatternsPos = (ctxt->commandPatternsPos + cmdAdvance) % patterns->commandPatternsSize;

		inByte = inByte>>4;

		// Get cmdRot and cmdAdvance.  Using the second nibble of the input.
		buffer = ctxt->commandAccum ^ inByte;
		cmdRot = (buffer & 0x1F);
		buffer = (ctxt->commandAccum >> 5) ^ ((bbt_hash_t)*inputPtr);
		cmdAdvance = (buffer & 0xFF) + 1;
		ctxt->commandAccum = BIT_ROTATE(ctxt->commandAccum, 8);

		// Apply second iteration to command generator.
		buffer= patterns->commandPatterns[ctxt->commandPatternsPos];
		if (cmdRot > 0) {
			buffer = BIT_ROTATE(buffer, cmdRot);
		}
		ctxt->commandAccum ^= buffer;
		ctxt->commandPatternsPos = (ctxt->commandPatternsPos + cmdAdvance) % patterns->commandPatternsSize;

		// Get hashRot and hashAdvance from the current commandAccum.
		hashRot = ctxt->commandAccum & 0x1F;
		hashAdvance = ((ctxt->commandAccum >> 5) & 0xFF) + 1;
		ctxt->commandAccum = BIT_ROTATE(ctxt->commandAccum, 8);

		// Calculate the next value on the hash using the pattern.
		ctxt->hashPatternsPos = (ctxt->hashPatternsPos + hashAdvance) % patterns->hashPatternsSize;
		buffer = patterns->hashPatterns[ctxt->hashPatternsPos];
		if (hashRot > 0) {
			buffer = BIT_ROTATE(buffer, hashRot);
		}
		ctxt->hashAccum ^= buffer;
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

