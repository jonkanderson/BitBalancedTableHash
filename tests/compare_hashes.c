/* This takes two hash methods and calculates the entropy for each bit 
 * position separately.  The position of least entropy is also determined 
 * in the report.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <gsl/gsl_sf_log.h>
#include <gsl/gsl_math.h>

/* Jenkin's hash.  This list of 20 numbers was randomly chosen using R. */
const unsigned jen_init_nums[] = {
	2118065031, 1708005758, 1704588144, 924421376, 2364985521, 
	552754112, 1350635658, 420655580, 3111761921, 1421260411, 
	506174658, 3293530468, 3940422494, 2378250901, 2733653678, 
	2857399545, 4099450769, 3345902600, 2597697023, 803379642};
extern unsigned jen_hash(unsigned char *k, unsigned length, unsigned initval);
#define JEN_INDEX 0

uint64_t calc_hash_jen(void *hash_info, unsigned char *str, unsigned length) {
	return jen_hash(str, length, jen_init_nums[JEN_INDEX]);
}

/* Bit-balanced Table hash */
#include "hash_bbt.h"
#define BBT_HASH_TABLE bbt_table_1
extern struct bbt_hash_table BBT_HASH_TABLE;

uint64_t calc_hash_bbt(void *hash_info, unsigned char *str, unsigned length) {
	bbt_hash_ctxt *ctxt = (bbt_hash_ctxt*)hash_info;
	uint64_t hash;
	bbt_hash_add(ctxt, str, length);
	hash = BBT_HASH_GET(ctxt);
	bbt_hash_reset(ctxt);
	return hash;
}

/* Structure to standardize the comparison. */
struct comparison_stats {
	uint64_t *ones_count;
	uint64_t tot_count;
	uint hash_width;
	uint64_t (*calc_hash)(void *hash_info, unsigned char *str, unsigned length);
	void *hash_info;
};

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
void report_comparison_stats(struct comparison_stats *cs, const char *name) {
	double p, h, min_h, sum_h, log_h;
	int i, min_i;
	uint64_t count = 0;
	uint64_t tot = cs->tot_count;

	sum_h = (double)0;
	printf("dat <- list(name=\"%s\", inputSize=%lu, stats=\"\n", name, tot);
	printf("\tindex, count, proportion, entropy, logHComplement\n");
	for (int i=0; i < cs->hash_width; i++) {
		count = cs->ones_count[i];
		p = (double)count/tot;
		h = -(p*gsl_sf_log(p) + (FLOAT_ONE-p)*gsl_sf_log(FLOAT_ONE-p))/M_LN2;
		if (i==0) {
			min_i = 0;
			min_h = h;
		} else {
			printf(" ");
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
		printf("\t%d, %lu, %0.10f, %0.10f, %0.10f\n", i, count, p, h, log_h);
		sum_h += h;
	}
	printf("\t\")\n");
	printf("\tprint(summary(read.csv(text=dat$stats)))\n");
	printf("\t# Minimum entropy is at position %i at %0.10f.\n", min_i, min_h);
	printf("\t# Average entropy is %0.10f.\n", sum_h / cs->hash_width);
	log_h = gsl_sf_log(FLOAT_ONE - min_h);
	printf("\t# logHComplement of minimum = ln(1-h) = %0.10f.\n", log_h);
	printf("\t# Predicted logHComplement from regression on random data = %1.10f\n", -1.035*gsl_sf_log(tot) + 3.562);
}

void free_comparison_stats(struct comparison_stats *cs) {
	free(cs->ones_count);
}

/* Strings from stdin are truncated to STR_BUFFER_SZ-1 */
#define STR_BUFFER_SZ 512

int main(int argc, char **argv) {
	struct comparison_stats jen_stats, bbt_stats;
	bbt_hash_ctxt bbt_hc;
	char b[STR_BUFFER_SZ];

	basic_init_comparison_stats(&jen_stats, 32);
	jen_stats.calc_hash = calc_hash_jen;

	bbt_hash_init(&bbt_hc, &(BBT_HASH_TABLE));
	basic_init_comparison_stats(&bbt_stats, BBT_HASH_WIDTH);
	bbt_stats.calc_hash = calc_hash_bbt;
	bbt_stats.hash_info = &bbt_hc;

	while (fgets(b, STR_BUFFER_SZ-1, stdin)) {
		process_comparison_stats(&jen_stats, (unsigned char *)b, strlen(b));
		process_comparison_stats(&bbt_stats, (unsigned char *)b, strlen(b));
	}

	report_comparison_stats(&jen_stats, "Jenkin's Hash");
	report_comparison_stats(&bbt_stats, "Bit-balanced Table Hash");

	free_comparison_stats(&jen_stats);
	free_comparison_stats(&bbt_stats);
	return 0;
}

