#!/usr/bin/env python3
""" test.py - test a given exponent

This will test the given exponent, and its mersenne number

"""

# imports
import sys

# read exponent
p = int(sys.argv[1])

# calc mersenne number
Mp = 2 ** p - 1

# sequence
Si = 4

for i in range(p - 2):

    Si = pow(Si, 2, Mp) - 2


if Si % Mp == 0:
    print (f"M{p} is prime!")
else:
    print (f"M{p} is not prime!")


