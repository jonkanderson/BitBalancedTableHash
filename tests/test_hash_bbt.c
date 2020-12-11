/*
 * This version uses the my new hash.
 * 
 * Read stdin line by line and hash it. The each byte position is written 
 * into five binary files for efficiency analysis. (The last is all combined.)
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hash_bbt.h"

#define HASH_TABLE bbt_table_1
extern struct bbt_hash_params HASH_TABLE;

#define STR_BUFFER_SZ 512

typedef union {
	struct {
		unsigned char x[4];
	} bytes;
	bbt_hash_t value;
} hash_bytes;

int main(int argc, char **argv) {
	hash_bytes h;
	char b[STR_BUFFER_SZ];
	bbt_hash_ctxt hc;

	FILE *f[5];
	for (int i=0; i<5; i++) {
		sprintf(b, "../out/mix-%d.dat", i);
		f[i] = fopen(b, "w");
	}

	bbt_hash_init(&hc, &(HASH_TABLE));

	while (fgets(b, STR_BUFFER_SZ-1, stdin)) {
		bbt_hash_calc(&hc, (unsigned char *)b, strlen(b));
		h.value = BBT_HASH_GET(&hc);
		for (int i=0; i<4; i++) {
			fputc(h.bytes.x[i], f[i]);
			fputc(h.bytes.x[i], f[4]);
		}
		bbt_hash_reset(&hc);
	}

	for (int i=0; i<5; i++) fclose(f[i]);

	return 0;
}

