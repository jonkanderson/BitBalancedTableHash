/* This library is used by compare_hashes_* tests.
 */

#ifndef COMPARISON_STATS_H
#define COMPARISON_STATS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <gsl/gsl_sf_log.h>
#include <gsl/gsl_math.h>

struct comparison_stats {
	uint64_t *ones_count;
	uint64_t tot_count;
	uint hash_width;
	uint64_t (*calc_hash)(void *hash_info, unsigned char *str, unsigned length);
	void *hash_info;
};

typedef enum {
	report_type_short,
	report_type_long } report_type;

void basic_init_comparison_stats(struct comparison_stats *cs, uint width);
void process_comparison_stats(struct comparison_stats *cs, unsigned char *str, unsigned sz);
void report_comparison_stats(struct comparison_stats *cs, const char *name, report_type rtyp);
void free_comparison_stats(struct comparison_stats *cs);

#endif

