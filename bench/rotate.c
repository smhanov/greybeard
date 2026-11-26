/* rotate.c -- Programming Pearls, Column 2: Aha! Algorithms.
 *
 * Rotate an array left by k: a[0..k) moves to the end, keeping order.
 *
 *   naive: rotate by one, k times        O(n*k)
 *   tuned: reverse head, reverse tail, reverse all   O(n), O(1) space
 *
 * The tuned version is not a faster loop. It is a different identity:
 * the reversal of a concatenation is the concatenation of the reversals.
 */
#include "bench.h"

#include <string.h>

#define N 100000u
#define K 10000u

static void rev(int *a, size_t lo, size_t hi) {   /* reverses [lo, hi) */
    while (lo + 1 < hi) {
        int t = a[lo];
        a[lo] = a[hi - 1];
        a[hi - 1] = t;
        lo++;
        hi--;
    }
}

static void rotate_naive(int *a, size_t n, size_t k) {
    for (size_t r = 0; r < k; r++) {
        int first = a[0];
        memmove(a, a + 1, (n - 1) * sizeof *a);
        a[n - 1] = first;
    }
}

static void rotate_reverse(int *a, size_t n, size_t k) {
    if (n == 0) return;
    k %= n;
    if (k == 0) return;
    rev(a, 0, k);        /* reverse the head  */
    rev(a, k, n);        /* reverse the tail  */
    rev(a, 0, n);        /* reverse the whole */
}

int main(void) {
    int *base = bench_alloc(N * sizeof *base);
    int *a    = bench_alloc(N * sizeof *a);
    int *b    = bench_alloc(N * sizeof *b);

    uint64_t s = 0x243F6A8885A308D3ULL;
    for (size_t i = 0; i < N; i++) base[i] = (int)(bench_rand(&s) & 0x7FFFFFFF);

    memcpy(a, base, N * sizeof *base);
    double t0 = bench_now();
    rotate_naive(a, N, K);
    double naive_ms = (bench_now() - t0) * 1000.0;

    memcpy(b, base, N * sizeof *base);
    t0 = bench_now();
    rotate_reverse(b, N, K);
    double tuned_ms = (bench_now() - t0) * 1000.0;

    int ok = memcmp(a, b, N * sizeof *base) == 0;

    printf("rotate: n=%u, k=%u\n", (unsigned)N, (unsigned)K);
    BENCH_LINE("rotate", (unsigned long long)N * K, naive_ms, tuned_ms,
               (double)(N * sizeof *base) / 1048576.0);
    printf("  same result: %s\n", ok ? "yes" : "NO -- BUG");

    free(base); free(a); free(b);
    return ok ? 0 : 1;
}
