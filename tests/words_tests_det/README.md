# Words Tests (deterministic)

## Make: `run_out_diffs`

Use this test to determine if hashes calculate the same as those saved in `out_bbt.txt`.  This test is expected to pass on any architecture that can create 64-bit hashes.  This test uses `words.txt` to generate `out_bbt.txt`.  The local file is compared to the generated file using `diff`. (The file `words.txt` is a random selection of 50,000 words from the system dictionary.)

## Make: `run_test_bbt` and `run_test_jen`

Use this test to detect hash conflicts.  They use all of the words found in the system dictionary.  There should be no conflicts.

## Make: `run_times`

Use this test to examine the running speed of the BBT hash by comparison to Jenkin's hash.  Currently, with default compiler optimization settings, Jenkin's will run 5.7x faster.  With full optimization, Jenkin's runs 3.4x faster.  These numbers are approximate.  This is useful for helping determine whether a coding change makes the hash calculate more efficiently.

