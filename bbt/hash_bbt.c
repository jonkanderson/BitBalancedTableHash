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

void bbt_hash_calc(bbt_hash_ctxt *ctxt, unsigned char *input, unsigned input_sz) {
	unsigned int cmdRot, cmdAdvance;
	unsigned int hashRot, hashAdvance;
	bbt_hash_t buffer;
	struct bbt_hash_patterns *patterns = ctxt->patterns;

	// Special case.  The empty string gets a hash value of 0.
	if (input_sz == 0) return;

	ctxt->inputSize += input_sz;

	// Work with a local context for efficiency.
	bbt_hash_ctxt tempCtxt;
	memcpy(&tempCtxt, ctxt, sizeof(bbt_hash_ctxt));

	unsigned char *inputPtr = input;
	for (unsigned i=0; i<input_sz; i++, inputPtr++) {
		// Input only influences the way that command accumulators change.
		bbt_hash_t inByte = *inputPtr;

		// Get cmdRot and cmdAdvance.  Using the nibble of input for commandA.
		buffer = tempCtxt.commandA.accum ^ (inByte & 0x0F);
		cmdRot = (buffer & 0x1F);
		buffer = (tempCtxt.commandA.accum >> 6) ^ (inByte & 0x0F);
		cmdAdvance = (buffer & 0x0F) + 1;
		tempCtxt.commandA.accum = BIT_ROTATE(tempCtxt.commandA.accum, 10);
	
		buffer= patterns->commandPatterns[tempCtxt.commandA.pos];
		if (cmdRot > 0) {
			buffer = BIT_ROTATE(buffer, cmdRot);
		}
		tempCtxt.commandA.accum ^= buffer;
		tempCtxt.commandA.pos = (tempCtxt.commandA.pos + cmdAdvance) % patterns->commandPatternsSize;

		inByte = inByte >> 4;

		// Get cmdRot and cmdAdvance.  Using the nibble of input for commandB.
		buffer = tempCtxt.commandB.accum ^ inByte;
		cmdRot = (buffer & 0x1F);
		buffer = (tempCtxt.commandB.accum >> 6) ^ inByte;
		cmdAdvance = (buffer & 0x0F) + 1;
		tempCtxt.commandB.accum = BIT_ROTATE(tempCtxt.commandB.accum, 10);
	
		buffer= patterns->commandPatterns[tempCtxt.commandB.pos];
		if (cmdRot > 0) {
			buffer = BIT_ROTATE(buffer, cmdRot);
		}
		tempCtxt.commandB.accum ^= buffer;
		tempCtxt.commandB.pos = (tempCtxt.commandB.pos + cmdAdvance) % patterns->commandPatternsSize;


		// Get hashRot and hashAdvance from the current command accumulators.
		buffer = tempCtxt.commandA.accum ^ tempCtxt.commandB.accum;
		hashRot = buffer & 0x1F;
		hashAdvance = ((buffer >> 5) & 0xFF) + 1;
		tempCtxt.commandA.accum = BIT_ROTATE(tempCtxt.commandA.accum, 8);
		tempCtxt.commandB.accum = BIT_ROTATE(tempCtxt.commandB.accum, 8);

		// Calculate the next value on the hash using the pattern.
		tempCtxt.hash.pos = (tempCtxt.hash.pos + hashAdvance) % patterns->hashPatternsSize;
		buffer = patterns->hashPatterns[tempCtxt.hash.pos];
		if (hashRot > 0) {
			buffer = BIT_ROTATE(buffer, hashRot);
		}
		tempCtxt.hash.accum ^= buffer;
	}

	// Copy work back from local context.
	memcpy(ctxt, &tempCtxt, sizeof(bbt_hash_ctxt));
}

bbt_hash_t bbt_hash_getHash(bbt_hash_ctxt *ctxt) {
	if (ctxt->inputSize == 0) { return 0; }
	bbt_hash_t retValue;
	bbt_hash_ctxt tempCtxt;
	memcpy(&tempCtxt, ctxt, sizeof(bbt_hash_ctxt));
	bbt_hash_calc(&tempCtxt, (unsigned char[sizeof(bbt_hash_ctxt)/2]){}, sizeof(bbt_hash_ctxt)/2);
	return BBT_HASH_GET(&tempCtxt);
}

