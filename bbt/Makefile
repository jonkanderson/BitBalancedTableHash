OUT_DIR=../out
CC=/usr/bin/clang

OBJS_FROM_LOCAL_SRC= \
	$(OUT_DIR)/hash_bbt.o \
	$(OUT_DIR)/hash_bbt_gen.o

CC_FLAGS=-g -fPIC

.PHONY: all
all: $(OBJS_FROM_LOCAL_SRC) $(OUT_DIR)/hash_tables.o

include settings.mk

$(OBJS_FROM_LOCAL_SRC) : $(OUT_DIR)/%.o : %.c %.h hash_bbt.h
	mkdir -p $(OUT_DIR)
	$(CC) -c $(CC_FLAGS) -o $@ $< -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)

$(OUT_DIR)/hash_tables.o: $(OUT_DIR)/hash_tables.c hash_bbt.h
	$(CC) -c $(CC_FLAGS) -o $@ -I . $(OUT_DIR)/hash_tables.c -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH)

$(OUT_DIR)/create_hash_params: create_hash_params.c $(OBJS_FROM_LOCAL_SRC)
	$(CC) $(CC_FLAGS) -o $@ -I . $^ -D BBT_HASH_WIDTH=$(BBT_HASH_WIDTH) -lgsl -lgslcblas

$(OUT_DIR)/hash_tables.c: $(OUT_DIR)/create_hash_params
	mkdir -p $(OUT_DIR)
	printf "#include \"hash_bbt.h\"\n" > $@
	exec $< bbt_table_1 37 13 $$(od -vAn -N4 -tu4 < /dev/urandom) >> $@
	exec $< bbt_table_2 41 17 $$(od -vAn -N4 -tu4 < /dev/urandom) >> $@
	exec $< bbt_table_3 177 23 $$(od -vAn -N4 -tu4 < /dev/urandom) >> $@
	exec $< bbt_table_4 271 41 $$(od -vAn -N4 -tu4 < /dev/urandom) >> $@

pristine:
	rm -rf $(OUT_DIR)
