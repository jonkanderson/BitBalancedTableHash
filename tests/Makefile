OUT_DIR=../out
CC=/usr/bin/clang

RANDOM_DATA_FILE=$(shell realpath ../data/RandomNumbers_20180911)

all: test6_compare
	@echo "Try 'make test6_big_compare' for a larger test."
	@echo "Try 'make test6_leaks' to test using valgrind."

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

pristine:
	rm -rf $(OUT_DIR)

$(OUT_DIR)/exp01: exp01.c
	mkdir -p $(OUT_DIR)
	$(CC) -o $@ $< -lgsl

test1_efficient: $(OUT_DIR)/exp01 $(OUT_DIR)/random_data
	$(OUT_DIR)/exp01 $(OUT_DIR)/random_data
	@echo "A random file."

$(OUT_DIR)/random_data:
	mkdir -p $(OUT_DIR)
	if [ -f  "$(RANDOM_DATA_FILE)" ] ; then ln -s "$(RANDOM_DATA_FILE)" $@ ; \
		else dd if=/dev/random of=$@ bs=512 count=2k ; fi

test1_not_efficient: $(OUT_DIR)/exp01
	@echo "A dictionary of words."
	$(OUT_DIR)/exp01 /usr/share/dict/words
	@echo "-----------------------."
	@echo "Yes."
	yes | dd count=5k | $(OUT_DIR)/exp01

#--------------------------------------
$(OUT_DIR)/hash_jen.o: hash_jen.c
	mkdir -p $(OUT_DIR)
	$(CC) -c -o $@ $<

$(OUT_DIR)/hash_bbt.o: ../bbt/hash_bbt.h 
	$(MAKE) -C ../bbt

$(OUT_DIR)/hash_tables.o: $(OUT_DIR)/hash_jen.o

#--------------------------------------
$(OUT_DIR)/hash02_basic: hash02_basic.c $(OUT_DIR)/hash_tables.o $(OUT_DIR)/hash_bbt.o
	mkdir -p $(OUT_DIR)
	$(CC) -o $@ $< -I ../bbt $(OUT_DIR)/hash_bbt.o $(OUT_DIR)/hash_tables.o

test2: $(OUT_DIR)/hash02_basic
	exec $<

#--------------------------------------
TEST_HASH_CMD=cat /usr/share/dict/words

$(OUT_DIR)/test_hash_jen: test_hash_jen.c 
	mkdir -p $(OUT_DIR)
	$(CC) -g -o $@ $<

test3-jen: $(OUT_DIR)/test_hash_jen $(OUT_DIR)/exp01
	$(TEST_HASH_CMD) | exec $<
	for E in $(patsubst %,$(OUT_DIR)/jen-%.dat,0 1 2 3 4) ; do ( cat $$E | $(OUT_DIR)/exp01) ; done

$(OUT_DIR)/test_hash_bbt: test_hash_bbt.c $(OUT_DIR)/hash_bbt.o $(OUT_DIR)/hash_tables.o ../bbt/hash_bbt.h 
	mkdir -p $(OUT_DIR)
	$(CC) -g -o $@ $< -I ../bbt $(OUT_DIR)/hash_bbt.o $(OUT_DIR)/hash_tables.o

test3-bbt: $(OUT_DIR)/test_hash_bbt $(OUT_DIR)/exp01
	$(TEST_HASH_CMD) | exec $<
	for E in $(patsubst %,$(OUT_DIR)/mix-%.dat,0 1 2 3 4) ; do ( cat $$E | $(OUT_DIR)/exp01) ; done

#--------------------------------------
TEST4_CMD=find /usr/include -maxdepth 4 -type f -exec cat {} \;

$(OUT_DIR)/to_small_strings: to_small_strings.c
	mkdir -p $(OUT_DIR)
	$(CC) -o $@ $<

$(OUT_DIR)/test4_data: $(OUT_DIR)/to_small_strings
	$(TEST4_CMD) | $< > $@-full
	cat $@-full | sort | uniq > $@
	nl $@ | tail -n 1

test4-jen: $(OUT_DIR)/test_hash_jen $(OUT_DIR)/exp01 $(OUT_DIR)/test4_data
	cat $(OUT_DIR)/test4_data | exec $<
	for E in $(patsubst %,$(OUT_DIR)/jen-%.dat,0 1 2 3 4) ; do ( cat $$E | $(OUT_DIR)/exp01) ; done

test4-bbt: $(OUT_DIR)/test_hash_bbt $(OUT_DIR)/exp01 $(OUT_DIR)/test4_data
	cat $(OUT_DIR)/test4_data | exec $<
	for E in $(patsubst %,$(OUT_DIR)/mix-%.dat,0 1 2 3 4) ; do ( cat $$E | $(OUT_DIR)/exp01) ; done

# This test4 reveals whether I am losing bits of entropy.
# It appears that I am very close to Jenkins hash now, but twice as slow.

# Test 5 is finding what to expect from randomness.  I am using 2M of 
# random data.  Unfortunately I am using the same data for each data 
# point which is wrong.
$(OUT_DIR)/test5_data.txt:
	for N in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 ; do ( \
		cat $$(find ../data/ -type f | sort) \
		| dd bs=1 count=$$(echo "100000*$$N" | bc ) \
		| $(OUT_DIR)/exp01 \
			) ; done > $@

#--------------------------------------
$(OUT_DIR)/compare_hashes: compare_hashes.c $(OUT_DIR)/hash_jen.o $(OUT_DIR)/hash_bbt.o $(OUT_DIR)/hash_tables.o
	mkdir -p $(OUT_DIR)
	$(CC) -g -o $@ -I $(OUT_DIR) -I ../bbt $^ -lgsl

test6_compare: $(OUT_DIR)/compare_hashes
	$(TEST_HASH_CMD) | exec $<

test6_big_compare: $(OUT_DIR)/compare_hashes $(OUT_DIR)/test4_data
	cat $(OUT_DIR)/test4_data | exec $<

# There are no leaks, but there are errors re. use of uninitialized value.
test6_leaks: $(OUT_DIR)/compare_hashes
	$(TEST_HASH_CMD) | head -n 500 | valgrind \
		--leak-check=full --track-origins=yes --show-error-list=yes \
		$<
