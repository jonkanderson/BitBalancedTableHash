ROOT_DIR=../..
OUT_DIR=$(ROOT_DIR)/out
TGT_DIR=$(OUT_DIR)/exhaustive_three_bytes

CC=/usr/bin/clang

BBT_HASH_WIDTH=64

OBJS_FROM_LOCAL_SRC= \
	$(OUT_DIR)/hash_bbt.o \
	$(OUT_DIR)/hash_bbt_gen.o \
	$(OUT_DIR)/hash_bbt_rng.o

CC_FLAGS=-g -fPIC

.PHONY: all
all:
	@echo "This test searches for hash conflicts in any combination of one to three bytes."
	@echo "The data files it produces amounts to 386 MB."
	@echo "Run the test by typing: make run_test_1"

.PHONY: proposed_tables
proposed_tables: $(TGT_DIR)/ht_proposed.c

#----------------------

$(OBJS_FROM_LOCAL_SRC) : $(OUT_DIR)/%.o :
	cd $(ROOT_DIR)/bbt ; $(MAKE)

$(OUT_DIR)/create_hash_patterns:
	cd $(ROOT_DIR)/bbt ; $(MAKE)

$(TGT_DIR)/ht.c :
	$(MAKE) $(TGT_DIR)/ht_proposed.c
	cp $(TGT_DIR)/ht_proposed.c $@

$(TGT_DIR)/ht.o : $(TGT_DIR)/ht.c
	mkdir -p $(TGT_DIR)
	$(CC) -c $(CC_FLAGS) -o $@ -I . -I $(ROOT_DIR)/bbt $< -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)

RANDOM_NUM_CMD=od -vAn -N4 -tu4 < /dev/urandom

# Tables have prime sizes.
$(TGT_DIR)/ht_proposed.c: $(OUT_DIR)/create_hash_patterns
	mkdir -p $(TGT_DIR)
	printf "#include \"hash_bbt.h\"\n" > $@
	printf "#include <stdlib.h>\n" >> $@
	exec $< bbt_table_0 5 7 $$($(RANDOM_NUM_CMD)) >> $@
	exec $< bbt_table_1 11 7 $$($(RANDOM_NUM_CMD)) >> $@
	exec $< bbt_table_2 37 41 $$($(RANDOM_NUM_CMD)) >> $@
	printf "struct bbt_hash_patterns *bbt_gen_patterns[] = {\n" >> $@
	for I in $$(seq 0 2) ; do printf "\t&bbt_table_%d,\n" $$I ; done >> $@
	printf "\tNULL };\n\n" >> $@

clean:
	rm -rf $(TGT_DIR)

#------------------------

$(TGT_DIR)/test_1 : $(TGT_DIR)/% : %.c $(TGT_DIR)/ht.o $(OUT_DIR)/hash_bbt.o
	$(CC) $(CC_FLAGS) -o $@ -I . -I $(ROOT_DIR)/bbt $^ \
		-D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)

.PHONY: run_test_1
run_test_1: $(TGT_DIR)/test_1
	mkdir -p $(TGT_DIR)/data
	$< $(TGT_DIR)/data/out
	cd $(TGT_DIR)/data ; for EA in $$(ls *.dat) ; do \
		( cat $$EA | cut -f 1 | sort | uniq -d ) ; \
		done | nl
