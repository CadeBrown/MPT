/* util.c - misc. utilities
 *
 */

#include "MPT-impl.h"


// allocate a buffer for enough bits
mpt_limb_t* mpt_alloc_bits(size_t bts) {
    return malloc(bts / 8 + MPT_LIMB_SIZE * 8);
}

// set the 'p'th mersenne number
void mpt_set_Mp(mpt_limb_t* Mp, int64_t p) {

    int64_t cp = p, i, k=0;
    
    while (cp >= MPT_LIMB_BITS) {
        // fill with 1s
        Mp[k++] = ~((mpt_limb_t)0);
        cp -= MPT_LIMB_BITS;
    }

    // now, build up the rest
    mpt_limb_t last_l = 0;

    // fill with '1's
    for (i = 0; i < cp; ++i) {
        last_l = (last_l << 1) | 1;
    }

    // last number
    Mp[k] = last_l;

}


// zero out 'data' (N is number of limbs)
void mpt_set_0(mpt_limb_t* data, int64_t N) {

    int64_t i;
    for (i = 0; i < N; ++i) {
        data[i] = 0;
    }
    
}

static const char hexchars[] = "0123456789abcdefg";

// convert a number to hex string
// 'sz' is the number of limbs 
// NOTE: output should hold at least (sz * MPT_HDPL + 4) bytes
void mpt_gethexstr(mpt_limb_t* Mp, int64_t sz, char* output) {

    int64_t i, j, k = 0;
    for (i = 0; i < sz; ++i) {
        mpt_limb_t word = Mp[i];
        for (j = 0; j < MPT_HDPL; ++j) {
            output[k++] = hexchars[word % 16];
            word /= 16;
        }
    }

    // NUL-terminate
    output[k] = '\0';

    // reverse output
    for (i = 0; i * 2 < k; ++i) {
        char t = output[i];
        output[i] = output[k - i - 1];
        output[k - i - 1] = t;
    }

}

static int gethexdig(char c) {
    /**/ if (c >= 'a' && c <= 'z') return 10 + c - 'a';
    else if (c >= 'A' && c <= 'Z') return 10 + c - 'A';
    else if (c >= '0' && c <= '9') return c - '0';
    return 0;
}

void mpt_sethexstr(mpt_limb_t* Mp, int64_t N, char* str) {

    // allow prefix
    if (str[0] == '0' && str[1] == 'x') str += 2;

    int64_t slen = strlen(str);
    int64_t i = slen - 1, j, k = 0;
    

    char tmps[256];
    while (i >= 0) {

        // get some hex digits
        Mp[k] = 0;
        j = 0;

        while (i >= 0 && j < MPT_HDPL) {
            tmps[j++] = str[i--];
        }

        j--;
        while (j >= 0) {
            Mp[k] = 16 * Mp[k] + gethexdig(tmps[j--]);
        }        

        k++;
    }
    
    while (k < N) Mp[k++] = 0;

}