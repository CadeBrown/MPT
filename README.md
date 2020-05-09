# MPT

MPT is a Mersenne Prime Tester, specifically it tests numbers which are 1 less than a power of 2.

## Building

Right now, I only compile on Linux. But, there are no external libraries at all, so compilation should be easy on other platforms too.

Just run `make`, and then run `./MPT`. Voila!


## Algorithms

Right now, the algorithms used are extremely basic. Eventually, I will port FFT and NTT-based multiplication into this project (this is the only speed missing right now)

