/*
Copyright (c) 2020-present Jon K. Anderson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "hash_bbt_gen.h"

// *** Local Prototypes

unsigned bbtgen_count_nib_bits(unsigned x);
unsigned long int bbtgen_getBalanced16(gsl_rng *rng);
void bbtgen_fillTable(gsl_rng *rng, bbt_hash_t *table, unsigned tableSz);
void bbtgen_writeTable(FILE *outStream, char *name, bbt_hash_t *table, unsigned tableSz);

// *** Local Functions

// (Not used) Get the number of bits contained in the lowest 4 bits.
// TODO: Use this to validate whether a table value has the right number of bits.
unsigned bbtgen_count_nib_bits(unsigned x) {
	unsigned char count[16] = {
		0, 1, 1, 2,  1, 2, 2, 3,
		1, 2, 2, 3,  2, 3, 3, 4 };
	return count[x & 0x0F];
}

// Generate a 16-bit number where half of the bits are 1.
unsigned long int bbtgen_getBalanced16(gsl_rng *rng) {
	unsigned char bits[16];

	unsigned char *p = bits;
	for (int i=0; i<16; i++, p++) {
		*p = i;
	}

	for (int i=0; i<8; i++) {
		int ix = i + (gsl_rng_get(rng) % (16-i));
		if (i != ix) {
			unsigned char tmp = bits[ix];
			bits[ix] = bits[i];
			bits[i] = tmp;
		}
	}

	unsigned long int value = 0;
	for (int i=0; i<8; i++) {
		value |= (1 << bits[i]);
	}
	return value;
}

void bbtgen_fillTable(gsl_rng *rng, bbt_hash_t *table, unsigned tableSz) {
	bbt_hash_t *p = table;
	for (unsigned i=0; i < tableSz; i++, p++) {
		bbt_hash_t num = 0;
		for (int j=0; j < (8*sizeof(bbt_hash_t)); j += 16) {
			num = (num << 16) | bbtgen_getBalanced16(rng);
		}
		*p = num;
	}
}

void bbtgen_writeTable(FILE *outStream, char *name, bbt_hash_t *table, unsigned tableSz) {
	char hex[] = "0123456789ABCDEF";

	fprintf(outStream, "  .%s = (bbt_hash_t[]){\n", name);
	bbt_hash_t *p = table;
	for (unsigned i=0; i < tableSz; i++, p++) {
		fprintf(outStream, "    0x");
		for (int j=8*sizeof(bbt_hash_t)-4; j >= 0; j -= 4) {
			fprintf(outStream, "%c", hex[((*p)>>j) & 0x0F]);
		}
		fprintf(outStream, "ul, //NOLINT\n");
	}
	fprintf(outStream, "  }");
}

// *** API Functions

struct bbt_hash_patterns *bbt_hash_create_patterns(unsigned hashPatternsSize, unsigned commandPatternsSize, unsigned seed) {
	gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937); /* Mersenne Twister */
	gsl_rng_set(rng, seed);

	struct bbt_hash_patterns *patterns = (struct bbt_hash_patterns *)malloc(sizeof(struct bbt_hash_patterns));
	patterns->hashPatternsSize = hashPatternsSize,
	patterns->hashPatterns = (bbt_hash_t*)malloc(hashPatternsSize * sizeof(bbt_hash_t));
	patterns->commandPatternsSize = commandPatternsSize,
	patterns->commandPatterns = (bbt_hash_t*)malloc(commandPatternsSize * sizeof(bbt_hash_t));

	bbtgen_fillTable(rng, patterns->hashPatterns, hashPatternsSize);
	bbtgen_fillTable(rng, patterns->commandPatterns, commandPatternsSize);

	gsl_rng_free(rng);
	return patterns;
}

void bbt_write_patterns(FILE *outStream, struct bbt_hash_patterns *patterns, char *varname) {
	fprintf(outStream, "struct bbt_hash_patterns %s = {\n", varname);

	fprintf(outStream, "  .hashPatternsSize = %u, //NOLINT\n", patterns->hashPatternsSize);
	bbtgen_writeTable(outStream, "hashPatterns", patterns->hashPatterns, patterns->hashPatternsSize);
	fprintf(outStream, ",\n");

	fprintf(outStream, "  .commandPatternsSize = %u, //NOLINT\n", patterns->commandPatternsSize);
	bbtgen_writeTable(outStream, "commandPatterns", patterns->commandPatterns, patterns->commandPatternsSize);
	fprintf(outStream, "};\n");
}

void bbt_hash_free_patterns(struct bbt_hash_patterns *patterns) {
	free(patterns->hashPatterns);
	free(patterns->commandPatterns);
	free(patterns);
}

