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
 * New hashing tables are easy to generate.
 */

#include "hash_bbt.h"

void bbt_hash_init(bbt_hash_ctxt *ctxt, struct bbt_hash_table *ht) {
	ctxt->table = ht;
	ctxt->pos = 0;
	ctxt->hash = 0;
}

void bbt_hash_reset(bbt_hash_ctxt *ctxt) {
	ctxt->pos = 0;
	ctxt->hash = 0;
}

typedef union {
	struct {
		unsigned char advance : 3;
		unsigned char shift : 5;
	} m1;
	struct {
		unsigned char advance1 : 1;
		unsigned char shift : 5;
		unsigned char advance2 : 2;
	} m2;
	struct {
		unsigned char shift2 : 2;
		unsigned char advance : 3;
		unsigned char shift3 : 3;
	} m3;
	struct {
		unsigned char shift1 : 1;
		unsigned char advance : 3;
		unsigned char shift4 : 4;
	} m4;
	unsigned char value;	
} mh_bit_select;

#define GET_METHOD(ATTR) (((ctxt->hash & table->ATTR)==0)?0:1)

void bbt_hash_add(bbt_hash_ctxt *ctxt, unsigned char *input, unsigned input_sz) {
	mh_bit_select bsel;
	unsigned int shift, advance;
	bbt_hash_t buffer;
	struct bbt_hash_table *table = ctxt->table;
	
	if (input_sz == 0) return;

	unsigned char *p = input;
	for (unsigned i=0; i<input_sz; i++, p++) {
		bsel.value = *p;

		/* I have tried various configurations of extracting advance
		 * and shift.  All four of these seem to work similarly. The 
		 * version I settled on is to use modulo and do a round-robin on 
		 * each. (i%4)
		 */
		//switch(1) {
		//switch(GET_METHOD(methodMask1)*2 + GET_METHOD(methodMask2)) {
		switch(i%4) {
		case 0:
			advance = bsel.m1.advance;
			shift = bsel.m1.shift;
			break;
		case 1:
			advance = (bsel.m2.advance2<<1) + bsel.m2.advance1;
			shift = bsel.m2.shift;
			break;
		case 2:
			advance = bsel.m3.advance;
			shift = (bsel.m3.shift2<<3) + bsel.m3.shift3;
			break;
		case 3:
			advance = bsel.m4.advance;
			shift = (bsel.m4.shift4<<1) + bsel.m4.shift1;
		}

		ctxt->pos = (ctxt->pos+advance+1) % table->size;
		buffer = table->patterns[ctxt->pos];
		if (shift == 0) {
			ctxt->hash ^= buffer;
		} else {
			buffer = (bbt_hash_t)(buffer<<shift) | (bbt_hash_t)(buffer>>(BBT_HASH_WIDTH-shift));
			ctxt->hash ^= buffer;
		}
	}
}

