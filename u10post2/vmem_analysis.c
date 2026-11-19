#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>

#define ALLOC_SIZE (64 * 1024 * 1024) // 64 MB

int global_init = 42;      // Segmento .data
int global_uninit;         // Segmento .bss

void print_stats(const char *etapa) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);

    FILE *f = fopen("/proc/self/statm", "r");
    long size_pages = 0, resident_pages = 0;
    if (f) {
        fscanf(f, "%ld %ld", &size_pages, &resident_pages);
        fclose(f);
    }
    long page_kb = sysconf(_SC_PAGESIZE) / 1024;

    printf("\n[%s]\n", etapa);
    printf("  Memoria Virtual (VSZ) : %8ld KB\n", size_pages * page_kb);
    printf("  Memoria Fisica  (RSS) : %8ld KB\n", resident_pages * page_kb);
    printf("  Minor Page Faults     : %8ld\n", ru.ru_minflt);
    printf("  Major Page Faults     : %8ld\n", ru.ru_majflt);
}

int main(void) {
    int local_var = 10; // Stack

    printf("=== Analisis de Memoria Virtual en Linux (PID: %d) ===\n", getpid());
    printf("Direcciones virtuales:\n");
    printf("  Codigo (main)         : %p\n", (void *)main);
    printf("  Data   (global_init)  : %p\n", (void *)&global_init);
    printf("  BSS    (global_uninit): %p\n", (void *)&global_uninit);
    printf("  Stack  (local_var)    : %p\n", (void *)&local_var);

    print_stats("1. Estado inicial");

    char *region = mmap(NULL, ALLOC_SIZE, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (region == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    printf("\n  Region mmap (64 MB)   : %p\n", (void *)region);
    print_stats("2. Despues de mmap(64MB) sin escribir (Lazy Allocation)");

    long page_size = sysconf(_SC_PAGESIZE);
    for (size_t i = 0; i < ALLOC_SIZE; i += page_size) {
        region[i] = 1; // Toca 1 byte por cada pagina de 4 KB
    }
    print_stats("3. Despues de escribir 1 byte por pagina (Demand Paging)");

    printf("\n=== Mapa de segmentos (/proc/self/maps) ===\n");
    FILE *maps = fopen("/proc/self/maps", "r");
    char line[256];
    int count = 0;
    while (maps && fgets(line, sizeof(line), maps) && count < 18) {
        printf("%s", line);
        count++;
    }
    if (maps) fclose(maps);

    munmap(region, ALLOC_SIZE);
    return 0;
}
