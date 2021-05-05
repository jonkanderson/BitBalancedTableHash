/*
 * This version uses the Jenkins hash.
 *
 * Read stdin line by line and hash it. Write the hash and the input to stdout.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

unsigned jen_hash(unsigned char *k, unsigned length, unsigned initval);

//const unsigned jen_init = 0x30229459u; // A random number.
const unsigned jen_init = 4291737839u; // A random number from R.

#define STR_BUFFER_SZ 512

int main(int argc, char **argv) {
	unsigned int hash;
	char b[STR_BUFFER_SZ];

	while (fgets(b, STR_BUFFER_SZ-1, stdin)) {
		unsigned n = strlen(b);
		b[n-1] = 0;
		hash = jen_hash((unsigned char *)b, strlen(b), jen_init);
		printf("%016X\t%s\n", hash, b);
	}

	return 0;
}

