/*
 * Test the effective difference of a single bit changing on the input.
 *
 * This test uses random data as input. A hash is calculates.  A bit is
 * changed on the input at one location. Another hash is calculated.
 * Both hashes are XOR'ed and sent to output as a binary stream. This
 * is repeated until the output length is reached.
 */

#include "hash_bbt.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define HASH_PARAMS bbt_table_1
extern struct bbt_hash_patterns HASH_PARAMS;

#define HASH_INPUT_SZ 32

int main(int argc, char **argv) {

	if (argc < 4) {
		printf("Usage: %s <outfile> <bit position> <output size> <numeric random seed>\n", argv[0]);
		return 1;
	}

	unsigned argPos = 1;
	FILE *outfile = fopen(argv[argPos++], "w");
	unsigned long int bitPos = strtoul(argv[argPos++], NULL, 10);
	if (bitPos > ((8*HASH_INPUT_SZ)-1)) {
		fprintf(stderr, "ERROR: bitPos too large. Beyond end of buffer length.\n");
		return 1;
	}
	unsigned long int outputLength = strtoul(argv[argPos++], NULL, 10);
	unsigned long int seed = strtoul(argv[argPos++], NULL, 10);

	unsigned char hashInput[HASH_INPUT_SZ];
	gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937); /* Mersenne Twister */
	gsl_rng_set(rng, seed);

	bbt_hash_ctxt hc;
	bbt_hash_init(&hc, &(HASH_PARAMS));

	unsigned bitIndex = bitPos/8;
	unsigned bitShift = 7 - (bitPos%8);
	bbt_hash_t bitMask = ((bbt_hash_t)1) << bitShift;

	printf("Bit change at index %u and shift %u.\n", bitIndex, bitShift);

	for (unsigned outputWritten = 0; outputWritten < outputLength;) {

		unsigned char *p = hashInput;
		for (unsigned i = 0; i < HASH_INPUT_SZ; i++) {
			*p = gsl_rng_get(rng) & 0xFF;
			p++;
		}

		bbt_hash_t hashA;
		bbt_hash_t hashB;

		bbt_hash_calc(&hc, hashInput, HASH_INPUT_SZ);
		hashA = bbt_hash_getHash(&hc);
		bbt_hash_reset(&hc);

		hashInput[bitIndex] = hashInput[bitIndex] ^ bitMask;

		bbt_hash_calc(&hc, hashInput, HASH_INPUT_SZ);
		hashB = bbt_hash_getHash(&hc);
		bbt_hash_reset(&hc);

		bbt_hash_t hashCombined = hashA ^ hashB;
		for (unsigned i = 0; i < sizeof(bbt_hash_t); i++) {
			fputc(hashCombined & 0xFF, outfile);
			hashCombined = hashCombined >> 8;
			outputWritten++;
		}
	}

	fclose(outfile);
	gsl_rng_free(rng);
	return 0;
}

