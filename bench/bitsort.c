/* bitsort.c -- Programming Pearls, Column 1: Cracking the Oyster.
 *
 * Sort ten million integers drawn from [0, 10^7).
 *
 *   naive: qsort the lot            n log n comparisons, 40 MB of ints
 *   tuned: one bit per value        O(n), 1.25 MB, sequential
 *
 * The tuned version is not a faster sort. It is a different representation:
 * the value IS the address, so ordering comes for free.
 */
#include "bench.h"

#define N    10000000u   /* numbers to sort */
#define MAXV 10000000u   /* values live in [0, MAXV) */

static int cmp_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

int main(void) {
    uint32_t *v    = bench_alloc(N * sizeof *v);
    uint32_t *copy = bench_alloc(N * sizeof *copy);
    uint32_t *out  = bench_alloc(N * sizeof *out);

    uint64_t s = 0x9E3779B97F4A7C15ULL;
    for (uint32_t i = 0; i < N; i++)
        v[i] = (uint32_t)(bench_rand(&s) % MAXV);

    /* ---- naive: sort a copy ---- */
    for (uint32_t i = 0; i < N; i++) copy[i] = v[i];
    double t0 = bench_now();
    qsort(copy, N, sizeof *copy, cmp_int);
    double qms = (bench_now() - t0) * 1000.0;

    /* ---- tuned: bitset, then walk it ---- */
    size_t words = (MAXV + 63) / 64;
    uint64_t *bits = bench_alloc(words * sizeof *bits);
    for (size_t i = 0; i < words; i++) bits[i] = 0;

    t0 = bench_now();
    for (uint32_t i = 0; i < N; i++)
        bits[v[i] >> 6] |= 1ULL << (v[i] & 63);

    size_t k = 0;
    for (size_t w = 0; w < words; w++) {
        uint64_t x = bits[w];
        while (x) {
            unsigned bit = (unsigned)__builtin_ctzll(x);
            out[k++] = (uint32_t)(w * 64u + bit);
            x &= x - 1;              /* clears the lowest set bit */
        }
    }
    double bms = (bench_now() - t0) * 1000.0;

    /* ---- verify: the distinct values must agree, in order ---- */
    int ok = 1;
    size_t j = 0;
    for (uint32_t i = 0; i < N; i++) {
        if (i > 0 && copy[i] == copy[i - 1]) continue;  /* keep one per run */
        if (j >= k || out[j] != copy[i]) { ok = 0; break; }
        j++;
    }
    if (j != k) ok = 0;

    printf("bitsort: %zu distinct values, %zu numbers\n", k, (size_t)N);
    BENCH_LINE("bitsort", N, qms, bms, (double)(words * 8) / 1048576.0);
    printf("  same result: %s\n", ok ? "yes" : "NO -- BUG");
    printf("  memory: qsort array %.1f MB vs bitset %.2f MB\n",
           (double)(N * sizeof *copy) / 1048576.0,
           (double)(words * 8) / 1048576.0);

    free(v); free(copy); free(out); free(bits);
    return ok ? 0 : 1;
}
