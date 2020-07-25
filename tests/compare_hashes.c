/* This takes two hash methods and calculates the entropy for each bit 
 * position separately.  The position of least entropy is also determined 
 * in the report.
 */

#include "comparison_stats.h"

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
#define BBT_HASH_TABLE bbt_table_3
extern struct bbt_hash_table BBT_HASH_TABLE;

uint64_t calc_hash_bbt(void *hash_info, unsigned char *str, unsigned length) {
	bbt_hash_ctxt *ctxt = (bbt_hash_ctxt*)hash_info;
	uint64_t hash;
	bbt_hash_add(ctxt, str, length);
	hash = BBT_HASH_GET(ctxt);
	bbt_hash_reset(ctxt);
	return hash;
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

	report_comparison_stats(&jen_stats, "Jenkin's Hash", report_type_long);
	report_comparison_stats(&bbt_stats, "Bit-balanced Table Hash", report_type_long);

	free_comparison_stats(&jen_stats);
	free_comparison_stats(&bbt_stats);
	return 0;
}

