/*
 * Use the hashing algorithm as a random number generator.  Use two hashing 
 * contexts.  The next pseudo-random unsigned char is taken from the hash of 
 * one of them and the rest of the hash is fed into the other. On each 
 * iteration the contexts switch roles.
 */

#include "hash_bbt.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PARAMS_1 bbt_table_2
#define PARAMS_2 bbt_table_3

extern struct bbt_hash_params PARAMS_1;
extern struct bbt_hash_params PARAMS_2;

unsigned char getNext(bbt_hash_ctxt *ctxtThis, bbt_hash_ctxt *ctxtOther) {
	bbt_hash_t num = BBT_HASH_GET(ctxtThis);
	unsigned char outByte = num & 0xFF;
	num = num >> 8;

	unsigned char buffer[sizeof(bbt_hash_t)];
	unsigned char *p = buffer;
	for (unsigned i=0; i < sizeof(bbt_hash_t)-1; i++, p++) {
		*p = num & 0xFF;
		num = num >> 8;
	}
	bbt_hash_calc(ctxtOther, buffer, sizeof(bbt_hash_t)-1);

	return outByte;
}

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

	bbt_hash_ctxt hcA;
	bbt_hash_ctxt hcB;

	bbt_hash_init(&hcA, &PARAMS_1);
	bbt_hash_init(&hcB, &PARAMS_2);

	bbt_hash_calc(&hcA, randomString, strlen((char*)randomString));

	int useA = 1;
	for(unsigned long int i=0; i < ouputLength; i++) {
		if (useA == 1) {
			fputc(getNext(&hcA, &hcB), stdout);
			useA = 0;
		} else {
			fputc(getNext(&hcB, &hcA), stdout);
			useA = 1;
		}
	}

	return 0;
}

