#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define CACHE_LINE 64
#define STRIDE (CACHE_LINE / sizeof(size_t))
#define ITERATIONS 20000000ULL

static inline uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

void benchmark_size(size_t bytes) {
    size_t n = bytes / sizeof(size_t);
    size_t *arr = aligned_alloc(CACHE_LINE, bytes);
    if (!arr) {
        perror("aligned_alloc");
        exit(1);
    }

    size_t num_lines = n / STRIDE;
    size_t *perm = malloc(num_lines * sizeof(size_t));
    for (size_t i = 0; i < num_lines; i++) perm[i] = i;

    srand(42);
    for (size_t i = num_lines - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        size_t tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }

    for (size_t i = 0; i < num_lines; i++) {
        size_t curr = perm[i] * STRIDE;
        size_t next = perm[(i + 1) % num_lines] * STRIDE;
        arr[curr] = next;
    }
    free(perm);

    // Calentamiento de caché
    size_t idx = 0;
    for (size_t i = 0; i < num_lines * 4; i++) {
        idx = arr[idx];
    }

    uint64_t t0 = now_ns();
    for (uint64_t i = 0; i < ITERATIONS; i++) {
        idx = arr[idx];
    }
    uint64_t t1 = now_ns();

    volatile size_t sink = idx;
    (void)sink;

    double ns_per_access = (double)(t1 - t0) / (double)ITERATIONS;
    const char *region = (bytes <= 32 * 1024) ? "L1d (<=32KB)" :
                         (bytes <= 512 * 1024) ? "L2  (<=512KB)" :
                         (bytes <= 32 * 1024 * 1024) ? "L3  (<=32MB)" : "RAM (>32MB)";

    printf("%8zu KB | %-14s | %6.2f ns/acceso\n", bytes / 1024, region, ns_per_access);
    free(arr);
}

int main(void) {
    size_t sizes_kb[] = {4, 16, 32, 64, 256, 512, 1024, 8192, 32768, 65536};
    size_t count = sizeof(sizes_kb) / sizeof(sizes_kb[0]);

    printf("=== Benchmark Jerarquia de Cache (AMD EPYC 7763) ===\n");
    printf("   Tamano   | Nivel Esperado | Latencia Media\n");
    printf("------------+----------------+-----------------\n");
    for (size_t i = 0; i < count; i++) {
        benchmark_size(sizes_kb[i] * 1024);
    }
    return 0;
}
