/* MPT.h - Mersenne Prime Tester header file
 *
 */

#pragma once
#ifndef MPT_H__
#define MPT_H__


/* std includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>



#include <time.h>
#include <sys/time.h>


/* config options */

// if defined, print out intermediate terms
//#define MPT_TRACE_TERMS


/* MPT types */


// different limb sizes

// unsigned 8 bit
//#define MPT_LIMB_U8

// unsigned 16 bit
//#define MPT_LIMB_U16

// unsigned 32 bit
//#define MPT_LIMB_U32

// unsigned 64 bit
#define MPT_LIMB_U64


#if   defined(MPT_LIMB_U8)

typedef uint8_t  mpt_limb_t;

#elif defined(MPT_LIMB_U16)

typedef uint16_t mpt_limb_t;

#elif defined(MPT_LIMB_U32)

typedef uint32_t mpt_limb_t;

#elif defined(MPT_LIMB_U64)

typedef uint64_t mpt_limb_t;

#else
#error Unknown MPT_LIMB_ type defined!
#endif


// size (in bytes) of a single limb
#define MPT_LIMB_SIZE (sizeof(mpt_limb_t))

// size (in bits) of a single limb
#define MPT_LIMB_BITS (8 * MPT_LIMB_SIZE)

// hex digits per limb
#define MPT_HDPL (2 * MPT_LIMB_SIZE)

// maximum value for any limb
#define MPT_LIMB_MAX ((mpt_limb_t)~((mpt_limb_t)0))


/* MPT functions */

// allocate (using malloc) a buffer large enough for 'bts' bits
// NOTE: Use 'free()' on the resulting buffer
mpt_limb_t* mpt_alloc_bits(size_t bts);

// Set 'data' to be all 0s
// 'N' is the number of limbs in data
void mpt_set_0(mpt_limb_t* data, int64_t N);

// set a given buffer to the 'p'th mersenne number (2^p - 1)
// NOTE: Must hold at least 'p' bits (use mpt_alloc_bits)
void mpt_set_Mp(mpt_limb_t* Mp, int64_t p);

// convert a number to hex string
// 'sz' is the number of limbs 
// NOTE: output should hold at least (sz * MPT_HDPL + 4) bytes
void mpt_gethexstr(mpt_limb_t* Mp, int64_t sz, char* output);

// Set a given MPT bignum from a hex string
void mpt_sethexstr(mpt_limb_t* Mp, int64_t N, char* str);

// Set A <- A + b
// Where 'A' has 'N' limbs
void mpt_addl(int64_t N, mpt_limb_t* A, mpt_limb_t b);

// Set A <- A - b
// Where 'A' has 'N' limbs
void mpt_subl(int64_t N, mpt_limb_t* A, mpt_limb_t b);

// squares a number:
// C = A^2
// Where 'A' has 'N' limbs, and 'C' has '2N' limbs
// NOTE: 'A' and 'C' must not overlap!
void mpt_sqr_naive(int64_t N, mpt_limb_t* A, mpt_limb_t* C);


// calculates:
// C = A (mod 2^p - 1)
// NOTE: does this in place
void mpt_mod2pm1(int64_t N, mpt_limb_t* A, mpt_limb_t* C, int64_t p, mpt_limb_t* Mp);


/* general utils */

// return the time since it started
double mpt_time();


#endif /* MPT_H__ */
