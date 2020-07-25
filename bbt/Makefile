OUT_DIR=../out
CC=/usr/bin/clang

# Synchronize BBT_HASH_WIDTH with the value in hash_bbt.h.
#BBT_HASH_WIDTH=32
BBT_HASH_WIDTH=64

.PHONY: all
all: $(OUT_DIR)/hash_bbt.o $(OUT_DIR)/hash_tables.o

$(OUT_DIR)/hash_bbt.o: hash_bbt.c hash_bbt.h
	mkdir -p $(OUT_DIR)
	$(CC) -c -o $@ $<

$(OUT_DIR)/hash_tables.c: hash_table.R
	mkdir -p $(OUT_DIR)
	R -f $< --args $(OUT_DIR)/table1.c bbt_table_1 37 $(BBT_HASH_WIDTH)
	R -f $< --args $(OUT_DIR)/table2.c bbt_table_2 64 $(BBT_HASH_WIDTH)
	R -f $< --args $(OUT_DIR)/table3.c bbt_table_3 177 $(BBT_HASH_WIDTH)
	R -f $< --args $(OUT_DIR)/table4.c bbt_table_4 271 $(BBT_HASH_WIDTH)
	cat $(patsubst %,$(OUT_DIR)/table%.c,1 2 3 4) > $(OUT_DIR)/hash_tables.c

$(OUT_DIR)/hash_tables.o: $(OUT_DIR)/hash_tables.c hash_bbt.h
	$(CC) -c -o $@ -I . $(OUT_DIR)/hash_tables.c

pristine:
	rm -rf $(OUT_DIR)
