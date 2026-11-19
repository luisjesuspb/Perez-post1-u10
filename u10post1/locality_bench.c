#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define N 4096

static inline double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
}

int main(void) {
    int (*mat)[N] = malloc(sizeof(int[N][N]));
    if (!mat) {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            mat[i][j] = (i + j) & 0xFF;
        }
    }

    volatile long long sum_row = 0;
    double t0 = now_ms();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sum_row += mat[i][j];
        }
    }
    double t1 = now_ms();

    volatile long long sum_col = 0;
    double t2 = now_ms();
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            sum_col += mat[i][j];
        }
    }
    double t3 = now_ms();

    double time_row = t1 - t0;
    double time_col = t3 - t2;

    printf("=== Localidad Espacial (Matriz %dx%d = %zu MB) ===\n", N, N, sizeof(int[N][N]) / (1024 * 1024));
    printf("Acceso por Filas (Row-major)   : %8.2f ms (suma=%lld)\n", time_row, sum_row);
    printf("Acceso por Columnas (Col-major): %8.2f ms (suma=%lld)\n", time_col, sum_col);
    printf("Factor de penalizacion (Speedup): %.2fx mas lento por columnas\n", time_col / time_row);

    free(mat);
    return 0;
}
