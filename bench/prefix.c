/* prefix.c -- Programming Pearls, Column 8: Algorithm Design Techniques.
 *
 * Maximum-sum contiguous subarray.
 *
 *   naive: try every pair of endpoints      O(n^2)
 *   tuned: Kadane, one pass                 O(n)
 *
 * No amount of code tuning rescues the left column. This is the rung below
 * code tuning, and it is worth 10^3x.
 */
#include "bench.h"

#define N 30000u

int main(void) {
    int *a = bench_alloc(N * sizeof *a);
    uint64_t s = 0xA4093822299F31D0ULL;
    for (size_t i = 0; i < N; i++)
        a[i] = (int)(bench_rand(&s) % 2001u) - 1000;   /* [-1000, 1000] */

    /* ---- naive: every start, every end ---- */
    long long best1 = 0;
    double t0 = bench_now();
    for (size_t i = 0; i < N; i++) {
        long long sum = 0;
        for (size_t j = i; j < N; j++) {
            sum += a[j];
            if (sum > best1) best1 = sum;
        }
    }
    double naive_ms = (bench_now() - t0) * 1000.0;

    /* ---- tuned: keep one running total; reset only when it stops paying ---- */
    t0 = bench_now();
    long long best2 = 0, cur = 0;
    for (size_t i = 0; i < N; i++) {
        cur += a[i];
        if (cur > best2) best2 = cur;
        if (cur < 0) cur = 0;          /* a negative prefix is never worth keeping */
    }
    double tuned_ms = (bench_now() - t0) * 1000.0;

    BENCH_LINE("maxsubarray", N, naive_ms, tuned_ms,
               (double)(N * sizeof *a) / 1048576.0);
    printf("  same result: %s (%lld)\n",
           best1 == best2 ? "yes" : "NO -- BUG", best1);

    free(a);
    return best1 == best2 ? 0 : 1;
}
