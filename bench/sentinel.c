/* sentinel.c -- Programming Pearls, Column 9: Code Tuning.
 *
 * Linear search, two ways:
 *   naive: test the bound every iteration
 *   tuned: park the key in a spare slot, so the loop ends on the key and the
 *          bound test happens once, outside the loop.
 *
 * The Makefile builds this file at four optimization levels and runs all four.
 * Pinned to one core, median of 7 runs on a modern x86-64:
 *
 *      -O0   1.38x     the trick works, exactly as published
 *      -O1   0.51x     the sentinel loop is not unrolled: it is 2x SLOWER
 *      -O2   1.02x     the compiler hoisted the bound test: a wash
 *      -O3   1.02x     still a wash
 *
 * Read that again, because it is the whole thesis of this repo:
 *
 *      a 1990s constant-factor trick is not portable across compilers
 *      or microarchitectures. It has to be re-measured where it ships.
 *
 * Bentley published this tuning in 1986 on a VAX. The VAX charged per
 * instruction. Your laptop does not, and your optimizer has opinions.
 */
#include "bench.h"

#ifndef OPT_LABEL
#define OPT_LABEL "unspecified"
#endif

static long find_naive(const int *a, size_t n, int key) {
    for (size_t i = 0; i < n; i++)
        if (a[i] == key) return (long)i;
    return -1;
}

/* a must have one spare slot at a[n]. */
static long find_sentinel(int *a, size_t n, int key) {
    a[n] = key;                       /* the sentinel */
    size_t i = 0;
    while (a[i] != key) i++;
    return (i < n) ? (long)i : -1;
}

int main(void) {
    /* 4 KB of keys: L1-resident, so the loop is ALU-bound and a saved
     * comparison is the only thing left to save. */
    size_t n = 1024, q = 200000;

    int *a = bench_alloc((n + 1) * sizeof *a);
    uint64_t s = 0xB5297A4D3F84E1C7ULL;
    for (size_t i = 0; i < n; i++) a[i] = (int)(bench_rand(&s) & 0x7FFFFFFF);

    int *keys = bench_alloc(q * sizeof *keys);
    for (size_t i = 0; i < q; i++) {
        uint64_t r = bench_rand(&s);
        keys[i] = (r & 1) ? a[r % n] : (int)(r & 0x7FFFFFFF);
    }

    long sum1 = 0, sum2 = 0;

    double t0 = bench_now();
    for (size_t i = 0; i < q; i++) sum1 += find_naive(a, n, keys[i]);
    double naive_ms = (bench_now() - t0) * 1000.0;

    t0 = bench_now();
    for (size_t i = 0; i < q; i++) sum2 += find_sentinel(a, n, keys[i]);
    double tuned_ms = (bench_now() - t0) * 1000.0;

    printf("  %-4s", OPT_LABEL);
    BENCH_LINE("sentinel", q, naive_ms, tuned_ms,
               (double)(n * sizeof *a) / 1048576.0);
    if (sum1 != sum2) printf("  same result: NO -- BUG\n");

    free(a);
    free(keys);
    return sum1 == sum2 ? 0 : 1;
}
