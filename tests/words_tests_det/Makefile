ROOT_DIR=../..
OUT_DIR=$(ROOT_DIR)/out
TGT_DIR=$(OUT_DIR)/words_tests_det

WORDS_FILE_LOCAL=words.txt
WORDS_FILE_SYS=/usr/share/dict/words

OBJS_FROM_LOCAL_SRC= \
	$(OUT_DIR)/hash_bbt.o

.PHONY: all
all:
	less README.md

include $(ROOT_DIR)/bbt/settings.mk

#----------------------

$(OBJS_FROM_LOCAL_SRC) : $(OUT_DIR)/%.o :
	cd $(ROOT_DIR)/bbt ; $(MAKE)

clean:
	rm -rf $(TGT_DIR)

#------------------------

$(TGT_DIR)/util_hash_bbt : $(TGT_DIR)/% : %.c $(OUT_DIR)/hash_bbt.o
	mkdir -p $(TGT_DIR)
	$(CC) $(CC_FLAGS) -o $@ -I . -I $(ROOT_DIR)/bbt $^ \
		-D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)

$(TGT_DIR)/hash_jen.o : $(TGT_DIR)/%.o : ../%.c
	mkdir -p $(TGT_DIR)
	$(CC) -c $(CC_FLAGS) -o $@ -I . -I $(ROOT_DIR)/bbt $^

$(TGT_DIR)/util_hash_jen : $(TGT_DIR)/% : %.c $(TGT_DIR)/hash_jen.o
	mkdir -p $(TGT_DIR)
	$(CC) $(CC_FLAGS) -o $@ -I . -I $(ROOT_DIR)/bbt $^

#------------------------

.PHONY: run_test_bbt run_test_jen
run_test_bbt run_test_jen : run_test_% : $(TGT_DIR)/util_hash_%
	cat $(WORDS_FILE_SYS) | $< > $(TGT_DIR)/out_$*.txt
	cat $(TGT_DIR)/out_$*.txt | cut -f 1 | sort | uniq -d | nl

.PHONY: send_to_null_bbt send_to_null_jen
send_to_null_bbt send_to_null_jen : send_to_null_% : $(TGT_DIR)/util_hash_%
	cat $(WORDS_FILE_LOCAL) | $< > /dev/null
	cat $(WORDS_FILE_LOCAL) | $< > /dev/null
	cat $(WORDS_FILE_LOCAL) | $< > /dev/null
	cat $(WORDS_FILE_LOCAL) | $< > /dev/null

.PHONY: run_times
run_times:
	echo "time $(MAKE) send_to_null_jen" | /usr/bin/bash
	echo "time $(MAKE) send_to_null_bbt" | /usr/bin/bash

.PHONY: run_out_diffs
run_out_diffs: $(TGT_DIR)/util_hash_bbt
	cat $(WORDS_FILE_LOCAL) | $< > $(TGT_DIR)/out_bbt.txt
	diff out_bbt.txt $(TGT_DIR)/out_bbt.txt

