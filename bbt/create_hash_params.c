/*
Copyright (c) 2020-present Jon K. Anderson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "hash_bbt_gen.h"

#include "string.h"

void print_usage(FILE *f, char *prog) {
	fprintf(f, "Usage: %s <varname> <patternTabSize> <shiftTabSize> <seed>\n", prog);
	fprintf(f, "Example: %s bbt_table 37 13 64 819607651\n", prog);
	fprintf(f, "Output goes to stdout.\n");
}

int main(int argc, char *argv[]) {
	char *varname;
	unsigned long int patternTabSize;
	unsigned long int shiftTabSize;
	unsigned long int seed;

	if (argc < 4) {
		fprintf(stderr, "ERROR: Incorrect number of parameters.\n");
		print_usage(stderr, argv[0]);
		return 1;
	}
	int argPos = 1;

	varname = argv[argPos++];

	patternTabSize = strtoul(argv[argPos++], NULL, 10);
	if (patternTabSize <= 1) {
		fprintf(stderr, "ERROR: patternTabSize must be greater than 1.\n");
		print_usage(stderr, argv[0]);
		return 1;
	}

	shiftTabSize = strtoul(argv[argPos++], NULL, 10);
	if (shiftTabSize <= 1) {
		fprintf(stderr, "ERROR: shiftTabSize must be greater than 1.\n");
		print_usage(stderr, argv[0]);
		return 1;
	}

	seed = strtoul(argv[argPos++], NULL, 10);
	if (seed <= 0) {
		fprintf(stderr, "ERROR: Use an unsigned decimal integer for the seed.\n");
		print_usage(stderr, argv[0]);
		return 1;
	}

	struct bbt_hash_params *params = bbt_hash_create_params(patternTabSize, shiftTabSize, seed);
	bbt_write_params(stdout, params, varname);
	bbt_hash_free_params(params);

	return 0;
}

