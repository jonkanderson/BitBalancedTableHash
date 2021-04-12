/*
Copyright (c) 2020-present Jon K. Anderson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * The concept of this hashing method is that there is a table of patterns. 
 * Each pattern is a randomly selected number where exactly half of the bits 
 * are 1's.  The input is treated as if it were a program to navigate moving 
 * through the table by performing bitwise shift and xor operators determined 
 * by each byte.  Only operations using the patterns are included in the hash.
 * The point is that the actual input data itself is never added into the hash 
 * directly because my hypothesis is that any subtle pattern in the input 
 * will degrade the entropy.  The process is very simple and fairly fast.  
 * New hashing parameters are easy to generate.
 */

#include "hash_bbt.h"

void bbt_hash_init(bbt_hash_ctxt *ctxt, struct bbt_hash_params *params) {
	ctxt->params = params;
	ctxt->hash = 0;
	ctxt->shiftSource = params->shifts[0];
	ctxt->inputSize = 0;
	ctxt->patternsPos = 0;
	ctxt->shiftsPos = 0;
}

void bbt_hash_reset(bbt_hash_ctxt *ctxt) {
	ctxt->hash = 0;
	ctxt->shiftSource = ctxt->params->shifts[0];
	ctxt->inputSize = 0;
	ctxt->patternsPos = 0;
	ctxt->shiftsPos = 0;
}

// BIT_ROTATE moves SHIFT bits from the high end to low.
#define BIT_ROTATE(HASH,SHIFT) ((bbt_hash_t)((HASH)<<(SHIFT)) | (bbt_hash_t)((HASH)>>(BBT_HASH_WIDTH - (SHIFT))))

void bbt_hash_calc(bbt_hash_ctxt *ctxt, unsigned char *input, unsigned input_sz) {
	unsigned int ssShift, ssAdvance;
	unsigned int hShift, hAdvance;
	bbt_hash_t buffer;
	struct bbt_hash_params *params = ctxt->params;

	if (input_sz == 0) return;

	ctxt->inputSize += input_sz;

	unsigned char *inputPtr = input;
	for (unsigned i=0; i<input_sz; i++, inputPtr++) {

		// Get hShift and hAdvance from the current shiftSource.
		hShift = ctxt->shiftSource & 0x1F;
		hAdvance = ((ctxt->shiftSource >> 5) & 0xFF) + 1;
		ctxt->shiftSource = BIT_ROTATE(ctxt->shiftSource, 8);

		// Calculate the next value on the hash using the pattern.
		ctxt->patternsPos = (ctxt->patternsPos + hAdvance) % params->patternTabSize;
		buffer = params->patterns[ctxt->patternsPos];
		if (hShift > 0) {
			buffer = BIT_ROTATE(buffer, hShift);
		}
		ctxt->hash ^= buffer;

		// Get ssShift and ssAdvance.  Input only influences the way that shiftSource changes.
		buffer = ctxt->shiftSource ^ ((bbt_hash_t)*inputPtr);
		ssShift = (buffer & 0x1F);
		buffer = (ctxt->shiftSource >> 6) ^ ((bbt_hash_t)*inputPtr);
		ssAdvance = (buffer & 0xFF) + 1;
		ctxt->shiftSource = BIT_ROTATE(ctxt->shiftSource, 8);

		// Prepare shiftSource for the next iteration using the shifts pattern.
		buffer= params->shifts[ctxt->shiftsPos];
		if (ssShift > 0) {
			buffer = BIT_ROTATE(buffer, ssShift);
		}
		ctxt->shiftSource ^= buffer;
		ctxt->shiftsPos = (ctxt->shiftsPos + ssAdvance) % params->shiftTabSize;
	}
}

