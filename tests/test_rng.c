/*
 * Use the hashing algorithm as a random number generator.  Use two hashing 
 * contexts.  The next pseudo-random unsigned char is taken from the hash of 
 * one of them and the rest of the hash is fed into the other. On each 
 * iteration the contexts switch roles.
 */

#include "hash_bbt_rng.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PARAMS_1 bbt_table_2
#define PARAMS_2 bbt_table_3

extern struct bbt_hash_params PARAMS_1;
extern struct bbt_hash_params PARAMS_2;

int main(int argc, char *argv[]) {
	unsigned long int ouputLength;
	unsigned char *randomString;

	if (argc < 2) {
		fprintf(stderr, "ERROR: Expecting two arguments.\n");
		fprintf(stderr, "Usage: %s <outputLength> <randomString>\n", argv[0]);
		return 1;
	}
	int argPos = 1;

	ouputLength = strtoul(argv[argPos++], NULL, 10);
	if (ouputLength <= 0) {
		fprintf(stderr, "ERROR: ouputLength must be greater than 0.\n");
		return 1;
	}

	randomString = (unsigned char *)argv[argPos++];

	bbt_rng_ctxt rng;

	// Initialize the rng using a phrase that will be hashed.
	bbt_rng_init(&rng, &PARAMS_1, &PARAMS_2, 0);
	bbt_rng_phrase(&rng, randomString, strlen((char*)randomString));

	// Alternate initialization is to use a number that provides the initial hash value.
	// bbt_rng_init(&rng, &PARAMS_1, &PARAMS_2, 53086951135934ul);

	for(unsigned long int i=0; i < ouputLength; i++) {
		bbt_hash_t temp = bbt_rng_next(&rng);
		for (unsigned j = 0; j < sizeof(bbt_hash_t); j++) {
			fputc(temp & 0xFF, stdout);
			temp = temp >> 8;
		}
	}

	return 0;
}

