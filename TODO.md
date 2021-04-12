# TODO List

- Clean up sloppy Makefiles
- Separate testing directories
	- Deterministic tests -- based on saved params
	- Deterministic test results -- for regression / unit testing
	- Non-deterministic tests on both saved params and newly generated params.
- Generate `bbt_hash_params` instance from C using the GSL.
	- Use Mersenne Twister `gsl_rng_alloc(gsl_rng_mt19937)` and `gsl_rng_get(rng)`
	- Generate C text from C as well.
