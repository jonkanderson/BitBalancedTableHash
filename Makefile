.PHONY: all
all:
	cd bbt ; $(MAKE)

#-------------------------------- House keeping
.PHONY: pristine
pristine:
	rm -rf out
