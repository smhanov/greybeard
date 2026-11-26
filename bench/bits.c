/* bits.c -- Programming Pearls, Column 9 (Code Tuning) and 10 (Squeezing Space).
 *
 * Count the set bits in ten million random 64-bit words, three ways.
 *
 *   naive:     shift once per bit           ~64 iterations per word
 *   kernighan: x &= x - 1, one per set bit   ~32 iterations per word
 *   builtin:   one instruction              1 iteration per word
 *
 * The 1983 trick and the 1961 instruction are both faster than the obvious
 * loop, and the instruction is not always the right answer: measure.
 */
#include "bench.h"

#define N 10000000u

int main(void) {
    uint64_t *w = bench_alloc(N * sizeof *w);
    uint64_t s = 0x082EFA98EC4E6C89ULL;
    for (size_t i = 0; i < N; i++) w[i] = bench_rand(&s);

    uint64_t c1 = 0, c2 = 0, c3 = 0;

    double t0 = bench_now();
    for (size_t i = 0; i < N; i++) {
        uint64_t x = w[i];
        while (x) { c1 += x & 1u; x >>= 1; }
    }
    double naive_ms = (bench_now() - t0) * 1000.0;

    t0 = bench_now();
    for (size_t i = 0; i < N; i++) {
        uint64_t x = w[i];
        while (x) { c2++; x &= x - 1; }    /* clears the lowest set bit */
    }
    double k_ms = (bench_now() - t0) * 1000.0;

    t0 = bench_now();
    for (size_t i = 0; i < N; i++)
        c3 += (uint64_t)__builtin_popcountll(w[i]);
    double b_ms = (bench_now() - t0) * 1000.0;

    BENCH_LINE("popcount", N, naive_ms, b_ms,
               (double)(N * sizeof *w) / 1048576.0);
    printf("  kernighan x&(x-1) %8.2f ms   %6.1fx vs naive\n",
           k_ms, bench_ratio(naive_ms, k_ms));
    printf("  same result: %s (%llu bits)\n",
           (c1 == c2 && c2 == c3) ? "yes" : "NO -- BUG",
           (unsigned long long)c1);

    free(w);
    return (c1 == c2 && c2 == c3) ? 0 : 1;
}
