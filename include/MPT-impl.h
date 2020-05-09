/* MPT-impl.h - internal helper file
 *
 */

#pragma once
#ifndef MPT_IMPL_H__
#define MPT_IMPL_H__

#include "MPT.h"


/* static kernel functions */

/* bitsize-dependent */


#if   defined(MPT_LIMB_U8)

// adds a+b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
// NOTE: 'hi' part will either be 0 or 1
static void mptl_add(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    *(uint16_t*)lohi = ((uint16_t)a) + ((uint16_t)b);
}

// subtracts a+b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
// NOTE: 'hi' part will either be 0 or 1
static void mptl_sub(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    *(uint16_t*)lohi = ((uint16_t)a) + ~((uint16_t)b - 1);
}

// multiplies a*b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
static void mptl_mul(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    *(uint16_t*)lohi = ((uint16_t)a) * ((uint16_t)b);
}

#elif defined(MPT_LIMB_U16)

// adds a+b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
// NOTE: 'hi' part will either be 0 or 1
static void mptl_add(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    *(uint32_t*)lohi = ((uint32_t)a) + ((uint32_t)b);
}


// subtracts a+b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
// NOTE: 'hi' part will either be 0 or 1
static void mptl_sub(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    *(uint32_t*)lohi = ((uint32_t)a) + ~((uint32_t)b - 1);
}


// multiplies a*b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
static void mptl_mul(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    *(uint32_t*)lohi = ((uint32_t)a) * ((uint32_t)b);
}

#elif defined(MPT_LIMB_U32)

// adds a+b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
// NOTE: 'hi' part will either be 0 or 1
static void mptl_add(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    *(uint64_t*)lohi = ((uint64_t)a) + ((uint64_t)b);
}


// subtracts a+b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
// NOTE: 'hi' part will either be 0 or 1
static void mptl_sub(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    *(uint64_t*)lohi = ((uint64_t)a) + ~((uint64_t)b - 1);
}


// multiplies a*b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
static void mptl_mul(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    *(uint64_t*)lohi = ((uint64_t)a) * ((uint64_t)b);
}


#elif defined(MPT_LIMB_U64)

// adds a+b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
// NOTE: 'hi' part will either be 0 or 1
static void mptl_add(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    lohi[0] = ((uint64_t)a) + ((uint64_t)b);
    lohi[1] = (lohi[0] < a || lohi[0] < b) ? 1 : 0;
}


// subtracts a+b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
// NOTE: 'hi' part will either be 0 or 1
static void mptl_sub(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    // on 32 bit, we can just alias to a u64
    lohi[0] = ((uint64_t)a) + ~((uint64_t)b - 1);
    lohi[1] = (lohi[0] > a) ? MPT_LIMB_MAX : 0;
}


// multiplies a*b, and calculates the low and high parts and stores in 'lohi', 
// i.e. lohi[0] == lo, and lohi[1] == hi
static void mptl_mul(mpt_limb_t a, mpt_limb_t b, mpt_limb_t* lohi) {
    uint64_t u1 = (a & 0xffffffff);
    uint64_t v1 = (b & 0xffffffff);
    uint64_t t = (u1 * v1);
    uint64_t w3 = (t & 0xffffffff);
    uint64_t k = (t >> 32);

    a >>= 32;
    t = (a * v1) + k;
    k = (t & 0xffffffff);
    uint64_t w1 = (t >> 32);

    b >>= 32;
    t = (u1 * b) + k;
    k = (t >> 32);

    lohi[1] = (a * b) + w1 + k;
    lohi[0] = (t << 32) + w3;
}


#endif




// Returns a limb which is in a bignum, but at a non-even multiple of MPT_LIMB_BITS,
// This method will return a limb of the bits A[offset:offset+MPT_LIMB_BITS],
// where offset can be between 0 and MPT_LIMB_BITS (if it's more, just offset 'A' before the call)
// Example: mptl_getodd(A, 0) returns A[0]
static mpt_limb_t mptl_getodd(mpt_limb_t* A, int64_t offset) {
    return (A[0] >> (offset)) | (A[1] << (MPT_LIMB_BITS - offset)); 
}

/* NUMBER THEORY */

// Returns the GCD (Greatest Common Denominator) of 'A' and 'B'
static int64_t mpt_gcd(int64_t a, int64_t b) {
    if (b == 0) {
        return a;
    } else {
        return mpt_gcd(b, a % b);
    }
}

// Return the GCD (Greatest Common Denominator) of 'A' and 'B'
// AND calculate numbers 'x' and 'y' such that:
//   x * a + y * b = gcd(a, b)
// NOTE: xy[0] is x, xy[1] is y
static int64_t mpt_egcd(int64_t a, int64_t b, int64_t* xy) {
	if (a == 0) {
        // base case
        xy[0] = 0;
        xy[1] = 1;
		return b;
	} else {
        // calculate a new 'xy' from reduction
        int64_t new_xy[2];
        int64_t gcd = mpt_egcd(b % a, a, new_xy);

        xy[0] = new_xy[1] - (b / a) * new_xy[0];
        xy[1] = new_xy[0];

        return gcd;
    }
}


// Return a^-1 (mod N)
// Or, return '0' if there was no inverse (i.e. it was not invertible)
static int64_t mpt_modinv(int64_t a, int64_t N) {
    int64_t xy[2];
    int64_t gcd = mpt_egcd(a, N, xy);
    if (gcd != 1) {
        // not invertible!
        return 0;
    } else {
        int64_t r = xy[0] % N;
        if (r < 0) r += N;
        return r;
    }
}

// Calculate a*b (mod m)
static uint64_t mpt_modmul(uint64_t a, uint64_t b, uint64_t m) {
    int64_t res = 0;
    while (a != 0) {
        if (a & 1) res = (res + b) % m;
        a >>= 1;
        b = (b << 1) % m;
    }
    return res;
}

// Calculate a^b (mod m)
static int64_t mpt_modpow(int64_t a, int64_t b, int64_t m) {
    if (b < 0) {
        int64_t a_inv = mpt_modinv(a, m);
        // no modular inverse
        if (a_inv == 0) return -1;

        // return it using positive branch
        return mpt_modpow(a_inv, -b, m);
    } else {
        // convert to the right range
        a %= m;
        if (a < 0) a += m;

        // result
        int64_t res = 1;

        // now, calculate using repeated squaring
        while (b > 0) {
            /*if (m == 4309503329) {
                printf("%lli::%lli,%lli\n", res, a, b);
            }*/
            if (b % 2 == 1) {
                // multiply by active bit
                res = mpt_modmul(res, a, m);
            }

            // repeated squaring
            a = mpt_modmul(a, a, m);
            b /= 2;
        }

        // ensure result is positive
        if (res < 0) res += m;
        return res;
    }
}


/// Internal miller rabin trial test
static bool i_milrab(int64_t n, int64_t a) {


    if (n % a == 0) return false;

    // Decompose: n = 2^r * d + 1
    int64_t r = 0;
    int64_t d = n - 1;

    // take out powers of 2
    while (d % 2 == 0) {
        r++;
        d /= 2;
    }

    // calculate a^d (mod n)
    int64_t x = mpt_modpow(a, d, n);

    if (x == 1 || x == n - 1) {
        // still might be prime
        return true;
    } else {
        int64_t i;
        // complete the test 'r-1' times
        for (i = 0; i < r - 1; ++i) {
            x = mpt_modmul(x, x, n);
            // still might be prime

            if (x == n - 1) return true;
        }
        // not prime, definitely composite
        return false;
    }
}

// return whether an integer is prime
static bool mpt_isprime(int64_t val) {
    // special cases
    if (val < 2) return 0;
    if (val < 4 || val == 5) return 1;
    if (val % 2 == 0 || val % 3 == 0) return 0;

    /**/ if (val < 2047) return i_milrab(val, 2);
    else if (val < 1373653) return i_milrab(val, 2) && i_milrab(val, 3);
    else if (val < 9080191) return i_milrab(val, 31) && i_milrab(val, 73);
    else if (val < 25326001) return i_milrab(val, 2) && i_milrab(val, 3) && i_milrab(val, 5);
    else if (val < 3215031751UL) return i_milrab(val, 2) && i_milrab(val, 3) && i_milrab(val, 5) && i_milrab(val, 7);
    else if (val < 4759123141UL) return i_milrab(val, 2) && i_milrab(val, 7) && i_milrab(val, 61);
    else if (val < 1122004669633UL) return i_milrab(val, 2) && i_milrab(val, 13) && i_milrab(val, 23) && i_milrab(val, 1662803);
    else if (val < 2152302898747UL) return i_milrab(val, 2) && i_milrab(val, 3) && i_milrab(val, 5) && i_milrab(val, 7) && i_milrab(val, 11);
    else if (val < 3474749660383UL) return i_milrab(val, 2) && i_milrab(val, 3) && i_milrab(val, 5) && i_milrab(val, 7) && i_milrab(val, 11) && i_milrab(val, 13);
    else if (val < 341550071728321UL) return i_milrab(val, 2) && i_milrab(val, 3) && i_milrab(val, 5) && i_milrab(val, 7) && i_milrab(val, 11) && i_milrab(val, 13) && i_milrab(val, 17);
    else {
        // assume not prime
        return 0;
    }
}




#endif /* MPT_IMPL_H__ */

