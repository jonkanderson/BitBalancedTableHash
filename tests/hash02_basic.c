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
extern struct bbt_hash_patterns HASH_TABLE;

#include "hash_jen.c"
const unsigned jen_init = 37428652;

int main(int argc, char **argv) {
	char *s_arr[] = (char*[]){
		"a",
		"b",
		"abc",
		"",
		"Hello...This is a bit of a longer string",
		NULL };

	uint32_t hJen;
	bbt_hash_t hBbt;
	bbt_hash_ctxt hc;

	bbt_hash_init(&hc, &(HASH_TABLE));

	int i=0;
	char *s;
	while ((s=s_arr[i++]) != NULL) {
		hJen = jen_hash((unsigned char *)s, strlen(s), jen_init);
		printf("J: %u \"%s\"\n", hJen, s);

		bbt_hash_calc(&hc, (unsigned char *)s, strlen(s));
		hBbt = bbt_hash_getHash(&hc);
		printf("M: %lu \"%s\"\n", hBbt, s);
		bbt_hash_reset(&hc);
	}
	return 0;
}

