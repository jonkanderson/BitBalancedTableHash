/*
 * Test the effective difference of a single bit changing on the input.
 * Evaluate each bit position on the first hash against all the bit 
 * positions on the other hash for any less than random distribution.
 * 
 * This tests for the "Bit Independence Criterion" of this hash.
 */

#include "hash_bbt.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define HASH_PARAMS bbt_table_1
extern struct bbt_hash_params HASH_PARAMS;

#define HASH_INPUT_SZ 32
//#define SAMPLE_SZ 1048576
//#define SAMPLE_SZ 100
#define SAMPLE_SZ 65536

#define HASH_BITS_COUNT (8*sizeof(bbt_hash_t))
#define COUNT_MATRIX_SZ (HASH_BITS_COUNT * HASH_BITS_COUNT)
#define BIT_ROTATE(HASH,SHIFT) ((bbt_hash_t)((HASH)<<(SHIFT)) | (bbt_hash_t)((HASH)>>(BBT_HASH_WIDTH - (SHIFT))))

int main(int argc, char **argv) {

	if (argc < 2) {
		printf("Usage: %d\n", argc);
		printf("Usage: %s <bit position> <numeric random seed>\n", argv[0]);
		return 1;
	}

	unsigned argPos = 1;
	unsigned long int bitPos = strtoul(argv[argPos++], NULL, 10);
	if (bitPos > 8*(HASH_INPUT_SZ-1)) {
		fprintf(stderr, "ERROR: bitPos too large. Beyond end of buffer length.\n");
		return 1;
	}
	unsigned long int seed = strtoul(argv[argPos++], NULL, 10);

	unsigned char hashInput[HASH_INPUT_SZ];
	gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937); /* Mersenne Twister */
	gsl_rng_set(rng, seed);

	bbt_hash_ctxt hc;
	bbt_hash_init(&hc, &(HASH_PARAMS));

	unsigned bitIndex = bitPos/8;
	unsigned bitShift = 7 - (bitPos%8);
	unsigned bitMask = 1 << bitShift;

	bbt_hash_t bitMatrix[HASH_BITS_COUNT];
	unsigned countMatrix[COUNT_MATRIX_SZ];
	memset(countMatrix, 0, COUNT_MATRIX_SZ * sizeof(unsigned));

	for (unsigned sampleCount = 0; sampleCount < SAMPLE_SZ; sampleCount++) {
		unsigned char *p = hashInput;
		for (unsigned i = 0; i < HASH_INPUT_SZ; i++) {
			*p = gsl_rng_get(rng) & 0xFF;
			p++;
		}

		bbt_hash_calc(&hc, hashInput, HASH_INPUT_SZ);
		bbt_hash_t hashA = bbt_hash_getHash(&hc);
		bbt_hash_reset(&hc);

		hashInput[bitIndex] = hashInput[bitIndex] ^ bitMask;

		bbt_hash_calc(&hc, hashInput, HASH_INPUT_SZ);
		bbt_hash_t hashB = bbt_hash_getHash(&hc);
		bbt_hash_reset(&hc);

		for (unsigned i = 0; i < HASH_BITS_COUNT; i++) {
			bitMatrix[i] = hashB ^ BIT_ROTATE(hashA, i);
		}

		for (unsigned i = 0; i < HASH_BITS_COUNT; i++) {
			bbt_hash_t hashC = bitMatrix[i];
			for (unsigned j = 0; j < HASH_BITS_COUNT; j++) {
				if ((hashC & 1) != 0) {
					countMatrix[j * HASH_BITS_COUNT + i] ++;
				}
				hashC = hashC >> 1;
			}
		}
	}

	printf("cat(\"Bit change at index %u and shift %u.\\n\")\n", bitIndex, bitShift);
	printf("sampleSize <- %u\n", SAMPLE_SZ);
	printf("cat(\"Sample size is %u.\\n\")\n", SAMPLE_SZ);
	printf("cat(\"Half of sample size is %u.\\n\")\n", SAMPLE_SZ/2);
	printf("data <- c(\n");
	for (unsigned i = 0; i < HASH_BITS_COUNT; i++) {
		for (unsigned j = 0; j < HASH_BITS_COUNT; j++) {
			printf("%u, ", countMatrix[j * HASH_BITS_COUNT + i]);
		}
		printf("\n");
	}
	printf("0)\n");
	printf("dataMatrix <- matrix(data[1:%lu], ncol=%lu, byrow=T)\n", COUNT_MATRIX_SZ, HASH_BITS_COUNT);
	printf("cat(\"Summary of the distribution of counts:\\n\")\n");
	printf("print(summary(as.vector(dataMatrix)))\n");
	printf("p <- dataMatrix / sampleSize\n");
	printf("q <- 1-p\n");
	printf("h <- -p*log(p, base=2) -q*log(q, base=2)\n");
	printf("cat(\"Summary of the distribution of H (entropy):\\n\")\n");
	printf("print(summary(as.vector(h)))\n");

	gsl_rng_free(rng);
	return 0;
}

