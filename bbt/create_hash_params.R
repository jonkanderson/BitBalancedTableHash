# This program produces random numbers that have the same number of 0s as 1s in binary.
# Example:
#   mkdir -p ../out ; R -f create_hash_params.R --args ../out/hash_table.c bbt_table 17 ; cat ../out/hash_table.c

args = commandArgs(trailingOnly=T)
print(args)
if (length(args) == 0) {
	cat("ERROR: Indicate the file to save the output.\n")
	quit(status=1)
}

out <- file(args[1], "w")
varname <- args[2]
count <- as.numeric(args[3])
bits <- as.numeric(args[4])

halfbits <- floor(bits/2)
hexDigits <- c("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F")

outLiteralNum <- function(bitList) {
	bin <- rep(0, bits)
	bin[bitList] <- 1
	cat(file=out, "0x")
	for (i in seq(1,bits,4)) {
		n <- bin[i]*8 + bin[i+1]*4 + bin[i+2]*2 + bin[i+3]
		cat(file=out, hexDigits[n+1])
	}
	if (bits == 32) {
		cat(file=out, "u")
	} else if (bits == 64) {
		cat(file=out, "ul")
	} else {
		cat("ERROR: Variable bits is not 32 or 64.\n")
		quit(status=1)
	}
}

cat(file=out, "#include \"hash_bbt.h\"\n")
cat(file=out, "struct bbt_hash_params ")
cat(file=out, varname)
cat(file=out, " = {\n")

m <- 
cat(file=out, "  .methodMask1 = ")
cat(file=out, outLiteralNum(sample(1:bits, size=1)))
cat(file=out, ",\n")
cat(file=out, "  .methodMask2 = ")
cat(file=out, outLiteralNum(sample(1:bits, size=1)))
cat(file=out, ",\n")

cat(file=out, "  .size = ")
cat(file=out, count)
cat(file=out, ",\n")

numsWritten <- 0
cat(file=out, "  .patterns = (bbt_hash_t[]){\n")
while (numsWritten < count) {
	cat(file=out, "    ")
	cat(file=out, outLiteralNum(sample(1:bits, size=halfbits)))
	cat(file=out, ",\n")
	numsWritten <- numsWritten + 1

# I lose entropy when I don't make the bits exactly even!

#	if (numsWritten < count) {
#		cat(file=out, "    ")
#		cat(file=out, outLiteralNum(sample(1:bits, size=halfbits-1)))
#		cat(file=out, ",\n")
#		numsWritten <- numsWritten + 1
#	}
#
#	if (numsWritten < count) {
#		cat(file=out, "    ")
#		cat(file=out, outLiteralNum(sample(1:bits, size=halfbits+1)))
#		cat(file=out, ",\n")
#		numsWritten <- numsWritten + 1
#	}
}
cat(file=out, "  }\n")
cat(file=out, "};\n")

cat(file=out, "\n\n")
close(out)
