/*
Copyright (c) 2020-present Jon K. Anderson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
Example use: ../out/create_hash_params bbt_table 37 13 64 > ../out/hash_table.c ; cat ../out/hash_table.c

varname <- args[2]
patternTabSize <- as.numeric(args[3])
shiftTabSize <- as.numeric(args[4])
bits <- as.numeric(args[5])

unsigned long int strtoul(const char *nptr, char **endptr, int base);

If  endptr is not NULL, strtoul() stores the address of the first invalid character in *endptr.  If there were
no digits at all, strtoul() stores the original value of nptr in *endptr (and returns 0).  In  particular,  if
*nptr is not '\0' but **endptr is '\0' on return, the entire string is valid.

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

