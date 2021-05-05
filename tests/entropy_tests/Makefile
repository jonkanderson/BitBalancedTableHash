# TODO: This Makefile needs to be re-worked.
ROOT_DIR=../..
OUT_DIR=$(ROOT_DIR)/out
TGT_DIR=$(OUT_DIR)/entropy_tests

RANDOM_DATA_FILE=$(shell realpath ../data/RandomNumbers_20180911)
WORDS_FILE_SYS=/usr/share/dict/words

all: test6_compare
	@echo "Try 'make test6_compare_one_char' to test strings of only one character."
	@echo "Try 'make test6_big_compare' for a larger test."
	@echo "Try 'make test6_leaks' to test using valgrind."

include $(ROOT_DIR)/bbt/settings.mk

all_early_tests:
	@echo "===================== Examine efficient data"
	$(MAKE) test1_efficient
	@echo "===================== Examine not efficient data"
	$(MAKE) test1_not_efficient
	@echo "===================== Test 2 (Very basic)"
	$(MAKE) test2
	@echo "===================== Jenkins hash"
	$(MAKE) test3-jen
	@echo "===================== My new hash"
	$(MAKE) test3-bbt

clean:
	rm -rf $(TGT_DIR)

$(TGT_DIR)/exp01: exp01.c
	mkdir -p $(TGT_DIR)
	$(CC) -o $@ $< -lgsl

test1_efficient: $(TGT_DIR)/exp01 $(TGT_DIR)/random_data
	$(TGT_DIR)/exp01 $(TGT_DIR)/random_data
	@echo "A random file."

$(TGT_DIR)/random_data:
	mkdir -p $(TGT_DIR)
	if [ -f  "$(RANDOM_DATA_FILE)" ] ; then ln -s "$(RANDOM_DATA_FILE)" $@ ; \
		else dd if=/dev/random of=$@ bs=512 count=2k ; fi

test1_not_efficient: $(TGT_DIR)/exp01
	@echo "A dictionary of words."
	$(TGT_DIR)/exp01 $(WORDS_FILE_SYS)
	@echo "-----------------------."
	@echo "Yes."
	yes | dd count=5k | $(TGT_DIR)/exp01

#--------------------------------------
$(OUT_DIR)/hash_bbt.o:
	$(MAKE) -C $(ROOT_DIR)/bbt $@

$(OUT_DIR)/hash_bbt_rng.o:
	$(MAKE) -C $(ROOT_DIR)/bbt $@

$(OUT_DIR)/hash_tables.o:
	$(MAKE) -C $(ROOT_DIR)/bbt $@

$(TGT_DIR)/hash_jen.o: ../hash_jen.c
	mkdir -p $(TGT_DIR)
	$(CC) -c -o $@ $<

#--------------------------------------
$(TGT_DIR)/hash02_basic: hash02_basic.c $(OUT_DIR)/hash_tables.o $(OUT_DIR)/hash_bbt.o $(TGT_DIR)/hash_jen.o
	mkdir -p $(TGT_DIR)
	$(CC) -o $@ $^ -I $(ROOT_DIR)/bbt \
		-D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)

test2: $(TGT_DIR)/hash02_basic
	exec $<

#--------------------------------------
TEST_HASH_CMD=cat $(WORDS_FILE_SYS)

$(TGT_DIR)/test_hash_jen: test_hash_jen.c $(TGT_DIR)/hash_jen.o
	mkdir -p $(TGT_DIR)
	$(CC) -g -o $@ $^

test3-jen: $(TGT_DIR)/test_hash_jen $(TGT_DIR)/exp01
	$(TEST_HASH_CMD) | exec $< "$(TGT_DIR)"
	for E in $(patsubst %,$(TGT_DIR)/jen-%.dat,0 1 2 3 4) ; do ( cat $$E | $(TGT_DIR)/exp01) ; done

$(TGT_DIR)/test_hash_bbt: test_hash_bbt.c $(OUT_DIR)/hash_bbt.o $(OUT_DIR)/hash_tables.o $(ROOT_DIR)/bbt/hash_bbt.h
	mkdir -p $(TGT_DIR)
	$(CC) -g -o $@ $< -I $(ROOT_DIR)/bbt \
		$(OUT_DIR)/hash_bbt.o $(OUT_DIR)/hash_tables.o -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)

test3-bbt: $(TGT_DIR)/test_hash_bbt $(TGT_DIR)/exp01
	$(TEST_HASH_CMD) | exec $< "$(TGT_DIR)"
	for E in $(patsubst %,$(TGT_DIR)/mix-%.dat,0 1 2 3 4) ; do ( cat $$E | $(TGT_DIR)/exp01) ; done

#--------------------------------------
TEST4_CMD=find /usr/include -maxdepth 4 -type f -exec cat {} \;

$(TGT_DIR)/to_small_strings: to_small_strings.c
	mkdir -p $(TGT_DIR)
	$(CC) -o $@ $<

$(TGT_DIR)/test4_data: $(TGT_DIR)/to_small_strings
	$(TEST4_CMD) | $< > $@-full
	cat $@-full | sort | uniq > $@
	nl $@ | tail -n 1

test4-jen: $(TGT_DIR)/test_hash_jen $(TGT_DIR)/exp01 $(TGT_DIR)/test4_data
	cat $(TGT_DIR)/test4_data | exec $< "$(TGT_DIR)"
	for E in $(patsubst %,$(TGT_DIR)/jen-%.dat,0 1 2 3 4) ; do ( cat $$E | $(TGT_DIR)/exp01) ; done

test4-bbt: $(TGT_DIR)/test_hash_bbt $(TGT_DIR)/exp01 $(TGT_DIR)/test4_data
	cat $(TGT_DIR)/test4_data | exec $< "$(TGT_DIR)"
	for E in $(patsubst %,$(TGT_DIR)/mix-%.dat,0 1 2 3 4) ; do ( cat $$E | $(TGT_DIR)/exp01) ; done

# This test4 reveals whether I am losing bits of entropy.
# It appears that I am very close to Jenkins hash now, but twice as slow.

# Test 5 is finding what to expect from randomness.  I am using 2M of
# random data.  Unfortunately I am using the same data for each data
# point which is wrong.
$(TGT_DIR)/test5_data.txt:
	for N in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 ; do ( \
		cat $$(find $(ROOT_DIR)/data/ -type f | sort) \
		| dd bs=1 count=$$(echo "100000*$$N" | bc ) \
		| $(TGT_DIR)/exp01 \
			) ; done > $@

#--------------------------------------
$(TGT_DIR)/comparison_stats.o: comparison_stats.c comparison_stats.h
	mkdir -p $(TGT_DIR)
	$(CC) -c -o $@ $<

COMPARE_HASHES_DEPS= \
	$(TGT_DIR)/comparison_stats.o \
	$(TGT_DIR)/hash_jen.o \
	$(OUT_DIR)/hash_bbt.o \
	$(OUT_DIR)/hash_tables.o

$(TGT_DIR)/compare_hashes: compare_hashes.c $(COMPARE_HASHES_DEPS)
	mkdir -p $(TGT_DIR)
	$(CC) -g -o $@ -I $(TGT_DIR) -I $(ROOT_DIR)/bbt $^ -lgsl -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)

test6_compare: $(TGT_DIR)/compare_hashes
	$(TEST_HASH_CMD) | exec $<

test6_big_compare: $(TGT_DIR)/compare_hashes $(TGT_DIR)/test4_data
	cat $(TGT_DIR)/test4_data | exec $<

$(TGT_DIR)/compare_hashes_one_char: compare_hashes_one_char.c $(COMPARE_HASHES_DEPS)
	mkdir -p $(TGT_DIR)
	$(CC) -g -o $@ -I $(TGT_DIR) -I $(ROOT_DIR)/bbt $^ -lgsl -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)

test6_compare_one_char: $(TGT_DIR)/compare_hashes_one_char
	$(TEST_HASH_CMD) | exec $<

test6_leaks: $(TGT_DIR)/compare_hashes
	$(TEST_HASH_CMD) | head -n 500 | valgrind \
		--leak-check=full --track-origins=yes \
		$<

#--------------------------------------
test_hin: $(TGT_DIR)/hin
	cat $(WORDS_FILE_SYS) | $(TGT_DIR)/hin

$(TGT_DIR)/hin: hash_stdin.c $(OUT_DIR)/hash_tables.o $(OUT_DIR)/hash_bbt.o
	mkdir -p $(TGT_DIR)
	$(CC) -o $@ $< -I $(ROOT_DIR)/bbt \
		$(OUT_DIR)/hash_bbt.o $(OUT_DIR)/hash_tables.o -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)
	@echo ">>> $@"

#--------------------------------------
TEMPFILE_CMD=tempfile -d /tmp -p file_rand -s .dat

$(TGT_DIR)/test_rng : $(TGT_DIR)/% : %.c $(OUT_DIR)/hash_bbt.o $(OUT_DIR)/hash_bbt_rng.o $(OUT_DIR)/hash_tables.o
	$(CC) -g -o $@ -I . -I $(ROOT_DIR)/bbt $^ -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)

run_test_rng: $(TGT_DIR)/test_rng
	X=$$($(TEMPFILE_CMD)) ; $< 10485760 Blals5drush+ > $$X ; /usr/bin/ent $$X ; echo "Random data file: $$X"

#--------------------------------------
RANDNUM_CMD=cat /dev/urandom | dd ibs=1 count=4 skip=20 2>/dev/null | od -tu4 | cut -b 9-

TEST7_DEPS= \
	$(OUT_DIR)/hash_bbt.o \
	$(OUT_DIR)/hash_tables.o

$(TGT_DIR)/test7_oneBit: test7_oneBit.c $(TEST7_DEPS)
	mkdir -p $(TGT_DIR)
	$(CC) -g -o $@ -I $(TGT_DIR) -I $(ROOT_DIR)/bbt $^ -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH) -lgsl -lgslcblas

TEST7_FILE_SZ=1048576

run_test7_oneBit: $(TGT_DIR)/test7_oneBit
	for BIT_POS in 0 1 2 31 32 33 63 64 65 ; do \
		FILE=$$($(TEMPFILE_CMD)) ; \
		SEED=$$($(RANDNUM_CMD)) ; \
		echo "Seed: $$SEED" ; \
		$< "$$FILE" $$BIT_POS $(TEST7_FILE_SZ) $$SEED ; \
		/usr/bin/ent "$$FILE" ; \
		rm "$$FILE" ; \
		done

#--------------------------------------
TEST8_DEPS= \
	$(OUT_DIR)/hash_bbt.o \
	$(OUT_DIR)/hash_tables.o

$(TGT_DIR)/test8_bitCascade: test8_bitCascade.c $(TEST8_DEPS)
	mkdir -p $(TGT_DIR)
	$(CC) -g -o $@ -I $(TGT_DIR) -I $(ROOT_DIR)/bbt $^ -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH) -lgsl -lgslcblas

run_test8_bitCascade: $(TGT_DIR)/test8_bitCascade
	SEED=$$($(RANDNUM_CMD)) ; \
		echo "Seed: $$SEED" ; \
		BIT_POS=10 ; \
		$< $$BIT_POS $$SEED > /tmp/out.R
	R -q -e 'source("/tmp/out.R")'

