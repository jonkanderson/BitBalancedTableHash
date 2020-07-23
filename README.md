# BitBalancedTableHash

This is an implementation of a hashing function that is based on a table of randomized unsigned integers where the binary representation of each number has an equal number of 1's versus 0's.  I am not considering this a [cryptographically safe](https://en.wikipedia.org/wiki/Cryptographic_hash_function) function.

## Prerequisites

This code was developed on Debian Linux.  It requires [R](https://en.wikipedia.org/wiki/R_(programming_language)) for generating a bitstring table.  It also requires the [GSL](https://en.wikipedia.org/wiki/GNU_Scientific_Library) C library for running tests using a logarithm function. The file `/usr/share/dict/words` is one source of input used in the tests.

## Build

There is a basic `Makefile` in the `tests` directory and in the `bbt` directory.  Typing `make` in the `tests` directory will build a new hash function and test it against a [Jenkins hash function](https://en.wikipedia.org/wiki/Jenkins_hash_function).  Other short tests can be performed by typing `make all_early_tests`.

## Background

[Hashing functions](https://en.wikipedia.org/wiki/Hash_function) is a significantly studied area of computer science.  They are used, in particular, for efficient management of a [hash table](https://en.wikipedia.org/wiki/Hash_table).  The Bit-Balanced Table Hash function was developed as an experiment for learning more about the entropy equation in [information theory](https://en.wikipedia.org/wiki/Entropy_(information_theory)).

## Motivation

Developing a hashing fuction is a tricky beast.  Any idea to [obfuscate](https://en.wikipedia.org/wiki/Obfuscation) the resulting hash may seem clever to the inventor but could potentially *lose* entropy rather than provide any gain.

The main principles of this hash were based on the following observations:

- The [bitwise operators](https://en.wikipedia.org/wiki/Bitwise_operation) *xor* and *rotate* are particularly safe as reduction operators when hashing.
- Input data is particularly *unsafe* for inclusion directly into a hash.  (Ideally, two inputs that are highly similar should produce hashes that are significantly different.)
- Bitstrings which are balanced with an equal number of 1's versus 0's will less likely contribute bias as to the number of 1's in the resulting hash.
- A table of balanced bitstrings could be used as the *only* source for contributing to the resulting hash.
- Input data could be interpreteted as byte-instructions.  So, in this implementation, each byte provides two pieces of information: *advance* and *shift*.  The *advance* is the number of slots to advance an index in the table with a minimum value of 1.  The *shift* idicates the number of bits to rotate the table value before including it into the hash.  The resulting value is then combined with the hash using an *xor* operation.

## Equations

The main purpose of this experiment was to gain a better understanding of the entropy equation.  Calculations of entropy were used to guide the implementation and the development of variations of this hash.  The entropy equation of interest is:

$$
h = -\sum_i{p_i\cdot\log{p_i}}
$$

Efficiency (&eta;) is entropy relative to the maximum expected bits of entropy.  It is a value between 0 and 1.

$$
\eta = \frac{h}{h_\text{max}}
$$

In developing this hash I found that the following calculation a rather useful metric for determining whether I am losing bits of entropy:

$$
\text{logHComplement} = \ln{(1-h)}
$$

Using an empirically derived regression, I used the above equation to determine if my entropy calculations were close to what should be expected by comparison to what purely random values produce.

## Conclusion

I am not sure what was most valuable from this experiment. The hashing function was developed only as a motivation for understanding entropy.  As such, the testing process was possibly more valuable than the hash itself.  Finding a reliable metric to determine whether I am losing entropy within the development process was particularly of interest.  Regarding the hash, uderstand that I implemented no comparison to Keccak or any other [cryptographic hash function](https://en.wikipedia.org/wiki/Cryptographic_hash_function).

Jon Anderson (jonkanderson@gmail.com)
