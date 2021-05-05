# BitBalancedTableHash

This is an implementation of a non-cryptographic hashing function. The function is evaluated using entropy equations.

## Prerequisites

This code was developed on Debian Linux.  It requires the [GSL](https://en.wikipedia.org/wiki/GNU_Scientific_Library) C library for running tests using a logarithm function and for random generation of hash parameters. The file `/usr/share/dict/words` is one source of input used in the tests.

## Build

There is a basic `Makefile` in the `bbt` directory.  Type `make` in the `bbt` directory to build the primary C libraries.

The `tests/entropy_tests` directory contains a variety of methods for evaluating the entropy of the hash.  Try `make test3-bbt` which creates a stream which is evaluated with the program `exp01.c` which uses the GSL library.  Try `make run_test_rng` which uses the `ent` program found in the Debian repositories to evaluate a file of bytes generated by the random number generator based on bbt.

## Background

The [hashing function](https://en.wikipedia.org/wiki/Hash_function) is a well-studied area of computer science.  They are used, in particular, for efficient management of a [hash table](https://en.wikipedia.org/wiki/Hash_table). Ideally, two inputs that are highly similar should produce hashes which are significantly different.

This Bit-Balanced Table Hash method was developed as an experiment for learning more about the entropy equation in [information theory](https://en.wikipedia.org/wiki/Entropy_(information_theory)).

## Motivation

Developing a hashing function is a tricky beast.  Any idea to [obfuscate](https://en.wikipedia.org/wiki/Obfuscation) the resulting hash may seem clever to the inventor but can often *lose* entropy rather than provide any gain.

The main principles of this hash were based on the following observations:

- The [bitwise operators](https://en.wikipedia.org/wiki/Bitwise_operation) *xor* and *rotate* are particularly safe as reduction operators when hashing.
- Input data is particularly *unsafe* for inclusion into a hash directly.
- Bitstrings which are balanced with an equal number of 1's versus 0's will less likely contribute bias into the resulting hash.
- It is feasible that a table of balanced bitstrings could be used as the *only* source for contributing to a resulting hash.
- Input data could be interpreted as byte-instructions to move the hashing machine along rather than being added to the machine directly.

So, in this hash implementation, each byte influences two pieces of information: *advance* and *rotate*.  The *advance* is the number of slots to advance an index in the table with a minimum value of 1.  The *rotate* indicates the number of bits to rotate the table value before including it into the hash.  The resulting value is then combined into the hash using an *xor* operation.  Each *xor* will toggle exactly half of the bits of the hash.

## Longer Description of the Algorithm

A BBT hashing machine consists of three parts: a hash generator and two command generators.  The command generators provides iterating commands for both itself and for the hash generator.  Each generator has two parts: an accumulator and a table index to its respective pattern table.  The pattern table is an array of numbers that are the same bit-width as the desired hash size.  In the current implementation, all values are bit-balanced on a 16-bit alignment.  (By *bit-balanced* I mean that there are an equal number of ones and zeros in its binary form.)  Iterating a generator requires two commands: an index advance and a bit rotation.

Input is received into a command generator one nibble at a time.  An input nibble influences the iterating commands for a command generator by using the XOR operator with the input and the accumulator.  The result is never saved into the accumulator and thus the input will never *spoil* the entropy in the series of accumulator states.  Each of the two command generators are iterated for each nibble of input.  An XOR combination of the two command generator accumulators is used to iterate the hash generator.

The two iterating commands are simple.  The index advance is a value of 1 or greater and is added to the respective table index modulo the size of the pattern table.  The value in the pattern table at that index will be used to modify the accumulator.  The other command is a bit rotation.  Its value is in the range of 0 to 31 which provides the number of bits to rotate the pattern.  The pattern is bit rotated and applied to the accumulator using the XOR operator.  Because the hash patterns are bit-balanced, it is guaranteed that half of the bits of the accumulator will be toggled on each iteration.

The accumulator of the command generator provides iterating commands for both the command generators and the hash generator.  Each time numeric values for commands are read, the appropriate command generator accumulator is bit rotated.

A completed hash is made by pushing a number of zeroes as input into the machine and then using the resulting accumulator of the hash generator.  The extra zeros provide enough input to ensure that the effects of the tail of the input has been worked through the machine.

## Equations for Testing Entropy

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

A variety of experiments were performed on the hash in the `tests` directory.  Most are non-deterministic.  Some compare the entropy of individual bits of the hash.  From all current observations, this hash appears to maintain entropy.  Even the random number generator which uses two hashing machines fed into each other seems to perform very well.

## Conclusion

My goal was to produce a decent hashing process which performed well and is useful for making general hash tables.  Actually, my *primary* purpose was to gain a better understanding of the entropy equations.  I have accomplished both better than my expectations.

As a disclaimer, understand that I had never intended to make a [cryptographic hash function](https://en.wikipedia.org/wiki/Cryptographic_hash_function).  Although this hashing method performs very well, I make no claim that it is cryptographically safe.  The method is simple and I would not be surprised to find a way to *break* the hash where input can be carefully constructed specifically to produce a target hash value.

Jon Anderson
