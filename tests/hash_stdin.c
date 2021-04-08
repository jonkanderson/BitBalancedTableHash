/*
 * This is just making some hash functions work.  In another test I will 
 * create a binary file of parts of hashes in order to test the entropy.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hash_bbt.h"

#define HASH_TABLE bbt_table_1
extern struct bbt_hash_params HASH_TABLE;

int main(int argc, char **argv) {

	bbt_hash_t hBbt;
	bbt_hash_ctxt hc;
	bbt_hash_ctxt hc_all;

	char strBuf[256];

	bbt_hash_init(&hc, &(HASH_TABLE));
	bbt_hash_init(&hc_all, &(HASH_TABLE));

	while (feof(stdin) == 0) {
		fgets(strBuf, 256, stdin);

		unsigned sz = strlen(strBuf);
		if (sz > 0 && strBuf[sz-1] == '\n') {
			strBuf[--sz] = '\0';
		}
		bbt_hash_calc(&hc, (unsigned char *)strBuf, sz);
		bbt_hash_calc(&hc_all, (unsigned char *)strBuf, sz);
		hBbt = BBT_HASH_GET(&hc);
		printf("%lu \"%s\"\n", hBbt, strBuf);
		bbt_hash_reset(&hc);
	}
	hBbt = BBT_HASH_GET(&hc_all);
	printf("%lu ALL\n", hBbt);

	return 0;
}

