/* This library is used by compare_hashes_* tests.
 */

#include "comparison_stats.h"

void basic_init_comparison_stats(struct comparison_stats *cs, uint width) {
	cs->tot_count = 0;
	cs->hash_width = width;
	cs->ones_count = (uint64_t*)calloc(width, sizeof(uint64_t));
	cs->calc_hash = NULL;
	cs->hash_info = NULL;
}

void process_comparison_stats(struct comparison_stats *cs, unsigned char *str, unsigned sz) {
	uint64_t hash;
	uint64_t mask = 1;
	hash = cs->calc_hash(cs->hash_info, str, sz);
	for (int i=0; i < cs->hash_width; i++) {
		if ((hash & mask) != 0) {
			cs->ones_count[i]++;
		}
		mask = mask<<1;
	}
	cs->tot_count++;
}

#define FLOAT_ONE ((double)1)
void report_comparison_stats(struct comparison_stats *cs, const char *name, report_type rtyp) {
	double p, h, min_h, sum_h, log_h;
	int i, min_i;
	uint64_t count = 0;
	uint64_t tot = cs->tot_count;

	sum_h = (double)0;
	if (rtyp == report_type_long) {
		printf("dat <- list(name=\"%s\", inputSize=%lu, stats=\"\n", name, tot);
		printf("\tindex, count, proportion, entropy, logHComplement\n");
	} else {
		printf("################# name=\"%s\"\n", name);
	}
	for (int i=0; i < cs->hash_width; i++) {
		count = cs->ones_count[i];
		p = (double)count/tot;
		h = -(p*gsl_sf_log(p) + (FLOAT_ONE-p)*gsl_sf_log(FLOAT_ONE-p))/M_LN2;
		if (i==0) {
			min_i = 0;
			min_h = h;
		} else {
			if (rtyp == report_type_long) printf(" ");
			if (h < min_h) {
				min_i = i;
				min_h = h;
			}
		}
		if (h < FLOAT_ONE) {
			log_h = gsl_sf_log(FLOAT_ONE - h);
		} else {
			log_h = -INFINITY;
		}
		if (rtyp == report_type_long) {
			printf("\t%d, %lu, %0.10f, %0.10f, %0.10f\n", i, count, p, h, log_h);
		}
		sum_h += h;
	}
	if (rtyp == report_type_long) {
		printf("\t\")\n");
		printf("\tprint(summary(read.csv(text=dat$stats)))\n");
	}
	printf("\t# Minimum entropy is at position %i at %0.10f.\n", min_i, min_h);
	printf("\t# Average entropy is %0.10f.\n", sum_h / cs->hash_width);
	log_h = gsl_sf_log(FLOAT_ONE - min_h);
	printf("\t# logHComplement of minimum = ln(1-h) = %0.10f.\n", log_h);
	printf("\t# Predicted logHComplement from regression on random data = %1.10f\n", -1.035*gsl_sf_log(tot) + 3.562);
}

void free_comparison_stats(struct comparison_stats *cs) {
	free(cs->ones_count);
}
