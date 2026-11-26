/* layout.c -- Programming Pearls, Column 6 (Performance) and 10 (Squeezing Space).
 *
 * Sum one field across ten million records.
 *
 *   AoS: struct { id, val, flags, reserved }  - 16 B per record, 160 MB walked
 *   SoA: two int arrays                       - 4 B per value read, 40 MB walked
 *
 * Same computation, same answer, one quarter of the memory traffic. The cache
 * does not care about your type system; it cares how many bytes you touch.
 */
#include "bench.h"

#define N 10000000u

typedef struct {
    int id;
    int val;
    int flags;
    int reserved;        /* the padding a real codebase grows into */
} Rec;

int main(void) {
    Rec  *aos = bench_alloc(N * sizeof *aos);
    int  *soa_id  = bench_alloc(N * sizeof *soa_id);
    int  *soa_val = bench_alloc(N * sizeof *soa_val);

    uint64_t s = 0x13198A2E03707344ULL;
    for (size_t i = 0; i < N; i++) {
        int id  = (int)(bench_rand(&s) & 0xFFFFF);
        int val = (int)(bench_rand(&s) & 0xFFFF);
        aos[i].id = id;
        aos[i].val = val;
        aos[i].flags = 0;
        aos[i].reserved = 0;
        soa_id[i] = id;
        soa_val[i] = val;
    }

    long long sum1 = 0, sum2 = 0;

    double t0 = bench_now();
    for (size_t i = 0; i < N; i++) sum1 += aos[i].val;
    double naive_ms = (bench_now() - t0) * 1000.0;

    t0 = bench_now();
    for (size_t i = 0; i < N; i++) sum2 += soa_val[i];
    double tuned_ms = (bench_now() - t0) * 1000.0;

    BENCH_LINE("layout", N, naive_ms, tuned_ms,
               (double)(N * sizeof(int)) / 1048576.0);
    printf("  same result: %s\n", sum1 == sum2 ? "yes" : "NO -- BUG");
    printf("  touched: AoS %.0f MB vs SoA %.0f MB\n",
           (double)(N * sizeof(Rec)) / 1048576.0,
           (double)(N * sizeof(int)) / 1048576.0);

    free(aos); free(soa_id); free(soa_val);
    return sum1 == sum2 ? 0 : 1;
}
