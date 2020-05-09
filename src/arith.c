/* arith.c - arithmetic for big numbers
 *
 */


#include "MPT-impl.h"

// Set A <- A + b
void mpt_addl(int64_t N, mpt_limb_t* A, mpt_limb_t b) {

    int64_t i;
    mpt_limb_t lohi[2];

    mptl_add(A[0], b, lohi);

    A[0] = lohi[0];

    // handle carry propogation
    if (lohi[1] > 0) {
        i = 1;
        // check while we need to carry
        while (A[i] == MPT_LIMB_MAX) {
            i++;
        }
        // add final carry bit
        A[i]++;

    }
}


// Set A <- A - b
void mpt_subl(int64_t N, mpt_limb_t* A, mpt_limb_t b) {

    int64_t i;
    mpt_limb_t lohi[2];

    mptl_sub(A[0], b, lohi);
    //printf("%i - %i == %i\n", (int)A[0], (int)b, (int)lohi[0]);

    A[0] = lohi[0];

    // handle carry propogation
    if (lohi[1] > 0) {
        i = 1;
        while (i < N-1 && A[i] <= MPT_LIMB_MAX - lohi[1]) {
            A[i] += MPT_LIMB_MAX;
            i++;
        }

        A[i] += MPT_LIMB_MAX;
    }
}

// calculate C=A^2, A[N], C[2N]
// uses the naive algo, O(N^2)
void mpt_sqr_naive(int64_t N, mpt_limb_t* A, mpt_limb_t* C) {
    // loop vars
    int64_t i, j, k;

    // temp vars
    mpt_limb_t Ai, AiAj, lohi_add[2], lohi_mul[2];

    // 0 out the results (we may use it for scratch space/carry)
    for (i = 0; i < 2 * N; ++i) C[i] = 0;

    /* main loop:
     *    A ...
     * A
     * .
     * .
     * .
     * 
     * View the problem as a matrix (as above), and we will notice that it is symetric
     * So, we only need N^2/2 direct ops
     * 
     */
    for (i = 0; i < N; ++i) {

        // TODO: split up in half
        for (j = 0; j < N; ++j) {
            // compute current entry
            k = i + j;

            // calculate A[i] * A[j]
            mptl_mul(A[i], A[j], lohi_mul);

            // add to C[k]
            mptl_add(C[k], lohi_mul[0], lohi_add);

            C[k++] = lohi_add[0];


            // compute carry
            mpt_limb_t carry = lohi_mul[1] + lohi_add[1];
            //printf("CARRY: %i\n", carry);

            // place the carry
            while (carry > 0) {
                mptl_add(C[k], carry, lohi_add);
                C[k] = lohi_add[0];

                carry = lohi_add[1];
                k++;
            }
        }

    }

}


// helper for the modulo
static void mod_h0() {

}


// helper function
static void h_mod2pm1(int64_t N, mpt_limb_t* A, mpt_limb_t* C, int64_t p, mpt_limb_t* Mp, int64_t* cbits) {
    // number of limbs that Mp takes up
    int64_t Mp_N = p / MPT_LIMB_BITS + 1;

    // nothing neccessary but to copy it
    if (N < Mp_N) {
        memcpy(C, A, MPT_LIMB_SIZE * N);
        return;
    }

    *cbits -= p;

    // create a 'mask' for which bits of Mp exist on Mp[Mp_N - 1]
    mpt_limb_t lmask = (1ULL << (p % MPT_LIMB_BITS)) - 1;

    // loop vars
    int64_t i, j;

    // temp vars
    mpt_limb_t Ai, Aip, carry, lohi[2];

    // offset indices
    int64_t i_Ao = p / MPT_LIMB_BITS, i_Am = p % MPT_LIMB_BITS;

    // 0 it out
    memset(C, 0, MPT_LIMB_SIZE * N);
    A[N] = 0;

    int maxI = 0;

    bool all1 = true;

    for (i = 0; i < N - Mp_N; ++i) {
        /**/ if (i > Mp_N - 1) Ai = 0;
        else if (i == Mp_N - 1) Ai = A[i] & lmask;
        else Ai = A[i];
        Aip = mptl_getodd(&A[i + i_Ao], i_Am);

        mptl_add(Ai, Aip, lohi);
        carry = lohi[1];
        mptl_add(C[i], lohi[0], lohi);
        C[i] = lohi[0];
        carry += lohi[1];
        
        all1 = all1 && ((i == Mp_N - 1) ? (C[i] & lmask == lmask) : (C[i] == MPT_LIMB_MAX));

        j = i + 1;
        while (carry > 0) {
            mptl_add(C[j], carry, lohi);
            C[j] = lohi[0];
            carry = lohi[1];
            maxI = ++j;
        }
    }

    if (maxI < i) maxI = i;

    // now, set rest to 0
    i = all1 ? 0 : maxI;

    //i = maxI;
    while (i < N) C[i++] = 0;

}

// calculate C = A % 2^p - 1
// C has 'N' limbs, as does 'A'
void mpt_mod2pm1(int64_t N, mpt_limb_t* A, mpt_limb_t* C, int64_t p, mpt_limb_t* Mp) {

    int64_t bits = p;
    h_mod2pm1(N, A, C, p, Mp, &bits);
    h_mod2pm1(N, C, A, p, Mp, &bits);
    h_mod2pm1(N, A, C, p, Mp, &bits);
    h_mod2pm1(N, C, A, p, Mp, &bits);


    memcpy(C, A, MPT_LIMB_SIZE * N);

    /*

    // original N
    int64_t oN = N;

    A[N] = 0;


    // already smaller than Mp, so no need to do modulo
    if (N < Mp_N) return;


    // the 'offset' section of 'A' (a minor index is also required)
    mpt_limb_t* Aoff = &A[Mp_N];

    // the number of bits off that 'Aoff' should be read from
    int64_t Aoff_mr = p % MPT_LIMB_BITS;


    i = 0;
    // keep removing a word from the top
    while (N > Mp_N+2 && A[Mp_N] > lmask) {

        Ai = A[i] & lmask;
        Aip = mptl_getodd(&A[i_Ao], i_Am);

        printf(" :: %x %x\n", Ai, Aip);

        mptl_add(Ai, Aip, lohi);

        //A[i] = Ai + Aip;
        A[i] = lohi[0];

        // ignore last digit, it has now been 'solved'
        N--;
    }


    // check if they were all 1s
    bool hasO = (A[Mp_N - 1] & lmask) == lmask;
    for (i = 0; hasO && i < N; ++i) {
        hasO = hasO && A[i] == MPT_LIMB_MAX;
    }

    for (i = (hasO) ? 0 : Mp_N; i < oN; ++i) {
        A[i] = 0;
    }
    */


    /*
    int64_t pN = p / MPT_LIMB_BITS + 1;

    // already smaller than 2^p - 1
    if (N < pN - 1) return;
    // TODO: check for when they are the same, and try and eliminate any zero values at the end of 'N'?

    // now, we can use the fact that:
    // A = (A mod 2^p) + floor(A / 2^p), in (mod 2^p - 1)
    // And further, that 'A mod 2^p' is just the first 'p' bits of 'A',
    // and 'floor(A / 2^p)' is the same thing as 'A' shifted over 'p' bits

    int64_t i, j;

    // temp vars
    mpt_limb_t Ai, Aip, lohi[2];

    // the mask on the last word that pertains to Mp
    mpt_limb_t lmask = (1ULL << (p % MPT_LIMB_BITS)) - 1;

    A[pN] &= lmask;
    for (i = pN + 1; i < N; ++i) A[i] = 0;

    // keep track if the output was all 1s
    bool hadOnes = true;

    for (i = 0; i < pN+1; ++i) {
        // essentially, we have:
        // A: abcdefgh... (each letter represents 1 bit)
        // And we know that (mod 2^p-1), the following numbers are equivalent:
        //
        // abcdefgh...
        //
        //   and
        //
        // abcdefgh...
        // fgh...
        // (if, p = 5, skip the first 5 on the second summation)
        //
        //

        // return the 2 offsets
        Ai = A[i];
        Aip = mptl_getodd(&A[i + p / MPT_LIMB_BITS], p % MPT_LIMB_BITS);

        //printf("Ai = %x %x\n", A[i + p / MPT_LIMB_BITS], A[i + p / MPT_LIMB_BITS + 1]);
        //printf("sum = %x\n", A[i + p / MPT_LIMB_BITS] + A[i + p / MPT_LIMB_BITS + 1]);

        // add them together
        mptl_add(Ai, Aip, lohi);

        // A = low
        A[i] = lohi[0];

        //printf("Ai = %x\n", A[i]);

        // now, continue carrying the hi bit
        if (lohi[1] > 0) {
            j = i + 1;
            // check while we need to carry
            while (A[j] == MPT_LIMB_MAX) {
                j++;
            }
            // add final carry bit
            A[j]++;
        }

        // ensure it was all ones
        if (i == pN - 1) {
            //printf("HAD_ONES: %i\n", A[i]);
            hadOnes = hadOnes && (A[i] & lmask) == lmask;
        } else {
            // normal detection
            hadOnes = hadOnes && (A[i] == MPT_LIMB_MAX);
        }
    }

    A[pN - 1] &= lmask;

    // reset pointer back to start
    // check for edge case where the result is Mp
    if (hadOnes) {
        i = 0;
        while (i < N) A[i++] = 0;
    }

    */

}
