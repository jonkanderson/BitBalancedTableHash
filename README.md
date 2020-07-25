# BitBalancedTableHash

This is an implementation of a non-cryptographic hashing function. The function is evaluated using the entropy equation and compared to a Jenkin's hash.

## Prerequisites

This code was developed on Debian Linux.  It requires [R](https://en.wikipedia.org/wiki/R_(programming_language)) for generating a bitstring table.  It also requires the [GSL](https://en.wikipedia.org/wiki/GNU_Scientific_Library) C library for running tests using a logarithm function. The file `/usr/share/dict/words` is one source of input used in the tests.

## Build

There is a basic `Makefile` in the `tests` directory and in the `bbt` directory.  Typing `make` in the `tests` directory will build a new hash function and test it against a [Jenkins hash function](https://en.wikipedia.org/wiki/Jenkins_hash_function).  Other short tests can be performed by typing `make all_early_tests`.

## Background

The [hashing function](https://en.wikipedia.org/wiki/Hash_function) is a well-studied area of computer science.  They are used, in particular, for efficient management of a [hash table](https://en.wikipedia.org/wiki/Hash_table). Ideally, two inputs that are highly similar should produce hashes which are significantly different.

This Bit-Balanced Table Hash function was developed as an experiment for learning more about the entropy equation in [information theory](https://en.wikipedia.org/wiki/Entropy_(information_theory)).

## Motivation

Developing a hashing function is a tricky beast.  Any idea to [obfuscate](https://en.wikipedia.org/wiki/Obfuscation) the resulting hash may seem clever to the inventor but can often *lose* entropy rather than provide any gain.

The main principles of this hash were based on the following observations:

- The [bitwise operators](https://en.wikipedia.org/wiki/Bitwise_operation) *xor* and *rotate* are particularly safe as reduction operators when hashing.
- Input data is particularly *unsafe* for inclusion into a hash directly.
- Bitstrings which are balanced with an equal number of 1's versus 0's will less likely contribute bias as to the number of 1's in the resulting hash.
- It is feasible that a table of balanced bitstrings could be used as the *only* source for contributing to a resulting hash.
- Input data could be interpreted as byte-instructions.

So, in this hash implementation, each byte provides two pieces of information: *advance* and *shift*.  The *advance* is the number of slots to advance an index in the table with a minimum value of 1.  The *shift* indicates the number of bits to rotate the table value before including it into the hash.  The resulting value is then combined into the hash using an *xor* operation.  Each *xor* will toggle exactly half of the bits of the hash.

## Equations

The main purpose of this experiment was to gain a better understanding of the entropy equation.  Calculations of entropy were used to guide the implementation and the development of variations of this hash.  The entropy equation of interest is:

$$
h = -\sum_i{p_i\cdot\log{p_i}}
$$

Efficiency (&eta;) is entropy relative to the maximum expected bits of entropy.  It has a value between 0 and 1.

$$
\eta = \frac{h}{h_\text{max}}
$$

In developing this hash I found the following calculation to be a rather useful metric for determining whether I am losing entropy:

$$
\text{logHComplement} = \ln{(1-\eta)}
$$

Using an empirically derived regression, I used the above equation to determine if my entropy calculations were close to what should be expected by comparison to purely random values.

## Results

Both this hash and the Jenkin's hash appear to be similar using the test `make test6_compare`.  With a dictionary of words as input, the test evaluates each hash by individually calculating the entropy related  to each bit position and then making note of the bit position with the minimum entropy.  The test `make test6_compare_one_char` is a more diabolical comparison.  It also evaluates individual bit positions, however the input is a sequence of single characters with progressively longer string lengths. Both hashes appear to perform similarly.  Two changes improved the performance of this hash:

- I included a line to convolute the input byte by performing an xor of the byte with the string position squared.
- Longer tables of prime length seem to perform better.

## Future

Appropriate statistical analysis is currently missing. For instance, performing confidence intervals on the *logHComplement* metric would be helpful.

Also missing are repeatable unit tests. Up to this point most of the testing output has been variable due to randomly generated tables.  (Yes, it is not exactly [TDD](https://en.wikipedia.org/wiki/Test-driven_development).  But as the code morphs, the results are non-deterministic.  Unit tests could be added which assert a minimum bounds however.)

## Conclusion

I am not sure what was most valuable from this experiment. The hashing function was developed as a motivation for understanding entropy.  As such, the testing process was possibly more valuable than the hash itself.  Finding a reliable metric to determine whether I am losing entropy within the development process was particularly of interest.

As a disclaimer regarding the hash, understand that I implemented no comparison to Keccak or any other [cryptographic hash function](https://en.wikipedia.org/wiki/Cryptographic_hash_function).

Jon Anderson (jonkanderson@gmail.com)
