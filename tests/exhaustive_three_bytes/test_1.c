/*
 * This program tests for issues with small differences that produce the
 * same hash.  This is an exhaustive search for hashes in every
 * combination of one to three bytes.
 *
 * Data is exported into 256 files based on the lowest byte of the hash.
 * Each file is run through the `sort` and `uniq` Unix tools to find
 * conflicts.  The total data came to 386 MB.
 *
 * With this variation of the algorithm, I sometimes get a hash conflict
 * with table sizes of hash 11 and command 7.  This is a *much* better
 * result for this test from an earlier version.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hash_bbt.h"

extern struct bbt_hash_patterns *bbt_gen_patterns[];
const unsigned htIndex = 1;

#define FILEPATH_BUFSIZ (BUFSIZ)

int main(int argc, char **argv) {
	char filepath[FILEPATH_BUFSIZ];

	if (argc < 2) {
		printf("Usage: %s <outfile prefix path>\n", argv[0]);
		return 1;
	}
	int argPos = 1;
	char *filePrefix = argv[argPos++];
	if (strlen(filePrefix) > (FILEPATH_BUFSIZ - 8)) {
		printf("Error: Length of path prefix is too long.\n");
		return 1;
	}
	printf("Sending output to %s\n", filePrefix);

	// Open data files.
	FILE *f[256];
	for (unsigned i = 0; i < 256; i ++) {
		sprintf(filepath, "%s-%02X.dat", filePrefix, i);
		f[i] = fopen(filepath, "w");
		if (f[i] == NULL) {
			printf("Error: Unable to open file: %s\n", filepath);
			return 1;
		}
	}

	unsigned char buf[] = { 0, 0, 0 };
	bbt_hash_ctxt hc;
	bbt_hash_t hash;

	bbt_hash_init(&hc, bbt_gen_patterns[htIndex]);

	// Hashes for one byte.
	for (unsigned i = 0; i < 256; i ++) {
		buf[0] = i;
		bbt_hash_calc(&hc, buf, 1);
		hash = bbt_hash_getHash(&hc);
		fprintf(f[hash & 0xFF], "%016lx\t%02X\n", hash, buf[0]);
		bbt_hash_reset(&hc);
	}

	// Hashes for two bytes.
	for (unsigned j = 0; j < 256; j ++) {
		buf[0] = j;
		for (unsigned i = 0; i < 256; i ++) {
			buf[1] = i;
			bbt_hash_calc(&hc, buf, 2);
			hash = bbt_hash_getHash(&hc);
			fprintf(f[hash & 0xFF], "%016lx\t%02X%02X\n", hash, buf[0], buf[1]);
			bbt_hash_reset(&hc);
		}
	}

	// Hashes for three bytes.
	for (unsigned k = 0; k < 256; k ++) {
		buf[0] = k;
		for (unsigned j = 0; j < 256; j ++) {
			buf[1] = j;
			for (unsigned i = 0; i < 256; i ++) {
				buf[2] = i;
				bbt_hash_calc(&hc, buf, 3);
				hash = bbt_hash_getHash(&hc);
				fprintf(f[hash & 0xFF], "%016lx\t%02X%02X%02X\n", hash, buf[0], buf[1], buf[2]);
				bbt_hash_reset(&hc);
			}
		}
	}

	// Close data files.
	for (unsigned i = 0; i < 256; i ++) {
		fclose(f[i]);
	}

	return 0;
}

