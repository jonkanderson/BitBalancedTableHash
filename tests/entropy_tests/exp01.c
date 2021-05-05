/*
 * Take the input stream and tally the distribution of 8-bit bytes.
 * Calculate the entropy.
 */

#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_sf_log.h>
#include <gsl/gsl_math.h>

int main(int argc, char **argv) {
	int tally[256];
	int ch;
	int n = 0;
	double h = 0;

	for (int i=0; i<256; i++) tally[i] = 0;
	FILE *in;

	printf ("argc = %d\n", argc);

	if (argc < 2) {
		printf ("Input from stdin.\n");
		in = stdin;
	} else {
		printf ("Input file: %s\n", argv[1]);
		in = fopen(argv[1], "r");
		if (in == NULL) {
			printf("BAD file: Usage: %s <random_file>\n", argv[0]);
			return 1;
		}
	}

	while ((ch = fgetc(in)) != EOF) {
		tally[ch] ++;
		n ++;
	}
	fclose(in);

	printf("Frequency by byte:");
	for (int i=0; i<256; i++) {
		printf(" %d", tally[i]);
	}
	printf("\n");
	printf("Input length = %d\n", n);

	for (int i=0; i<256; i++) {
		double p = (double)(tally[i])/n;
		if (p>0) {
			h = h - p*gsl_sf_log(p);
		}
	}
	// Divide by ln(2) to produce base 2 logarithms to calculate bits of entropy.
	h = h/M_LN2;
	printf("Entropy (h) = %1.10f\n", h);

	// Maximum entropy on bytes is 8 bits. Efficiency is calculated by dividing by the max.
	double eta = h/8;
	printf("Efficiency (eta) = %1.10f\n", eta);
	printf("ln(1-eta) = %1.10f\n", gsl_sf_log(1-eta));

	// This prediction is based on the regression from test5.
	printf("Predicted value = %1.10f\n", -1.035*gsl_sf_log(n) + 3.562);

	return 0;
}

