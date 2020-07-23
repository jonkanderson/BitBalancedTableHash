.PHONY: all
all:
	cd tests ; $(MAKE)

#-------------------------------- House keeping
.PHONY: pristine
pristine:
	rm -rf out
