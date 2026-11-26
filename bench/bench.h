/* bench.h -- the only infrastructure this repo needs.
 *
 * Every benchmark prints one line: naive, tuned, the ratio, and the footprint.
 * Nothing is asserted about speed that this file did not time.
 */
#ifndef GREYBEARD_BENCH_H
#define GREYBEARD_BENCH_H

/* clock_gettime is POSIX, not ISO C: ask for it explicitly so -std=c11 compiles. */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline double bench_now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec * 1e-9;
}

/* xorshift64: deterministic, so two runs of the bench compare. */
static inline uint64_t bench_rand(uint64_t *s) {
    uint64_t x = *s;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return *s = x;
}

/* FNV-1a over raw bytes: cheap proof that the two versions agree. */
static inline uint64_t bench_hash(const void *p, size_t n) {
    const unsigned char *b = (const unsigned char *)p;
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; i++) {
        h ^= b[i];
        h *= 1099511628211ULL;
    }
    return h;
}

static inline void *bench_alloc(size_t bytes) {
    void *p = malloc(bytes);
    if (!p) {
        fprintf(stderr, "greybeard: out of memory for %zu bytes\n", bytes);
        exit(1);
    }
    return p;
}

static inline double bench_ratio(double naive_ms, double tuned_ms) {
    return naive_ms / (tuned_ms > 1e-9 ? tuned_ms : 1e-9);
}

#define BENCH_LINE(label, n, naive_ms, tuned_ms, mb)                         \
    printf("  %-11s n=%-9llu  naive %9.2f ms   tuned %8.2f ms   %6.1fx"     \
           "   %6.1f MB\n",                                                  \
           (label), (unsigned long long)(n), (double)(naive_ms),             \
           (double)(tuned_ms), bench_ratio((naive_ms), (tuned_ms)),           \
           (double)(mb))

#endif /* GREYBEARD_BENCH_H */
