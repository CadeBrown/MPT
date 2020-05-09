/* MPT.c - tests mersenne primes
 *
 */

#include "MPT-impl.h"


// test 2^p - 1
// This method uses Lucas-Lehmer test (LL) and the Number Theoretic Transform (NTT) to do the squaring at each step,
//   and some modular division tricks for 'mod Mp'
// ASSUMPTIONS:
//   'p' is prime
bool mpt_T_ntt0(int64_t p) {
    return false;
}


bool mpt_T_basic0(int64_t p) {
    // special case
    if (p == 2) return true;
    
    // p must be prime
    if (!mpt_isprime(p)) return false;

    if (p < MPT_LIMB_BITS) fprintf(stderr, "[MPT_warn]: Exponent '%lli' is smaller than the bits per limb (%lli). This may cause problems\n", (long long int)p, (long long int)MPT_LIMB_BITS);


    // number of limbs in the main sequence
    int64_t N = p / MPT_LIMB_BITS + 1;

    mpt_limb_t* Mp = mpt_alloc_bits(p);
    mpt_set_Mp(Mp, p);

    // allocate 'S_i', the current term in the sequence
    // NOTE: allocate twice as many bits as are required, so we can do in place modular arithmetic
    mpt_limb_t* S_i = mpt_alloc_bits((2 * N) * MPT_LIMB_BITS);
    mpt_limb_t* S_it = mpt_alloc_bits((2 * N) * MPT_LIMB_BITS);

    // set S_i = 4 to begin
    mpt_set_0(S_i, 2 * N);
    S_i[0] = 4;

    // only allocate if its a trace build
    #ifdef MPT_TRACE_TERMS
        char* tmp = malloc(2 * N * MPT_HDPL + 4);

        mpt_gethexstr(Mp, N, tmp);
        printf("M%i: 0x%s\n", p, tmp);
    #endif


    // current trial (beginning at 0)
    int64_t i;
    for (i = 0; i < p - 2; ++i) {

        #ifdef MPT_TRACE_TERMS
            mpt_gethexstr(S_i, N, tmp);
            printf("S%i: 0x%s\n", i, tmp);
        #endif

        // calculate the next term in the sequence:
        // S_i <- S_i ^ 2 - 2 (mod Mp)
        mpt_sqr_naive(N, S_i, S_it);

        #ifdef MPT_TRACE_TERMS
            mpt_gethexstr(S_it, 2 * N, tmp);
            printf("  s^2    : 0x%s\n", tmp);
        #endif

        mpt_subl(2 * N, S_it, 2);

        #ifdef MPT_TRACE_TERMS
            mpt_gethexstr(S_it, 2 * N, tmp);
            printf("  s^2-2  : 0x%s\n", tmp);
        #endif
        
        mpt_mod2pm1(2 * N, S_it, S_i, p, Mp);

        #ifdef MPT_TRACE_TERMS
            mpt_gethexstr(S_i, 2 * N, tmp);
            printf("  s^2-2mM: 0x%s\n", tmp);
        #endif


    }

    #ifdef MPT_TRACE_TERMS
        mpt_gethexstr(S_i, N, tmp);
        printf("S(final): 0x%s\n", tmp);
    #endif


    bool hasNZ = false;

    // ensure all are zero
    for (i = 0; i < N; ++i) {
        if (S_i[i] != 0) {
            hasNZ  = true;
            break;
        }
    }

    // free resources
    free(S_i);
    free(S_it);
    free(Mp);

    #ifdef MPT_TRACE_TERMS
    free(tmp);
    #endif

    // all were, thus it is prime
    return !hasNZ;

}

// get the start time (initialize it in 'ks_init')
static struct timeval mpt_start_time = (struct timeval){ .tv_sec = 0, .tv_usec = 0 };

// return the time since it started
double mpt_time() {
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    return (curtime.tv_sec - mpt_start_time.tv_sec) + 1.0e-6 * (curtime.tv_usec - mpt_start_time.tv_usec);
}


int main(int argc, char** argv) {
    gettimeofday(&mpt_start_time, NULL);

/*
    mpt_limb_t* x = malloc(10000), *y = malloc(10000);
    int64_t N = 32;

    mpt_sethexstr(x, N, "0x01eead7d8cc884082d709944");
    char* tmp = malloc(MPT_HDPL * N + 4);

    // mine:    0x3bbe2061e43b75b3f496d941e2b18d7687ba85f425a10
    //          0x3bbe2061e43b75f1ca468adaf332a3249ae305f425a10
    mpt_gethexstr(x, N, tmp);
    printf("x: 0x%s\n", tmp);

    mpt_sqr_naive(N / 2, x, y);

    mpt_gethexstr(y, N, tmp);
    printf("y: 0x%s\n", tmp);
*/

    /*int64_t i;
    for (i = 3; i < 1e9; ++i) {
    */
    int64_t p = 21701;
    double st = mpt_time();
    bool isp = mpt_T_basic0(p);
    st = mpt_time() - st;
    if (isp) {
        printf("M%i is prime! (%.3lfms/iter)\n", p, 1000.0 * st / (p - 2));
    }
    //}

}


