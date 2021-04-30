/*
 * This version uses the Jenkins hash.
 *
 * Read stdin line by line and hash it. The each byte position is written
 * into five binary files for efficiency analysis. (The last is all combined.)
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hash_jen.c"
//const unsigned jen_init = 0xFED43055u; // A random number.
//const unsigned jen_init = 0x9E8B884Au; // A random number.
//const unsigned jen_init = 2849719; // Random typing.
//const unsigned jen_init = 29974966; // Random typing.
//const unsigned jen_init = 0x738A2510u; // A random number.
const unsigned jen_init = 0x30229459u; // A random number.
//const unsigned jen_init = 4291737839u; // A random number from R.

#define STR_BUFFER_SZ 512

typedef union {
	struct {
		unsigned char x[4];
	} bytes;
	unsigned int value;
} hash_bytes;

int main(int argc, char **argv) {
	hash_bytes h;
	char b[STR_BUFFER_SZ];

	FILE *f[5];
	for (int i=0; i<5; i++) {
		sprintf(b, "../out/jen-%d.dat", i);
		f[i] = fopen(b, "w");
	}

	while (fgets(b, STR_BUFFER_SZ-1, stdin)) {
		h.value = jen_hash((unsigned char *)b, strlen(b), jen_init);
		for (int i=0; i<4; i++) {
			fputc(h.bytes.x[i], f[i]);
			fputc(h.bytes.x[i], f[4]);
		}
	}

	for (int i=0; i<5; i++) fclose(f[i]);
	return 0;
}

