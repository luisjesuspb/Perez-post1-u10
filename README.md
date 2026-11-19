# Perez-post1-
cd /workspaces/Perez-post1-u10

cat << 'EOF' > README.md
# Unidad 10: Jerarquía de Caché y Memoria Virtual en Linux

**Autor:** Luis Jesús Pérez (`luisjesuspb`)  
**Entorno de Ejecución:** Ubuntu Linux (GitHub Codespaces) — AMD EPYC 7763 64-Core Processor

---

## Estructura del Repositorio

```text
Perez-post1-u10/
├── u10post1/
│   ├── cache_bench.c        # Benchmark de latencia por niveles de caché (Pointer Chasing)
│   └── locality_bench.c     # Benchmark de localidad espacial (Row-major vs. Column-major)
├── u10post2/
│   └── vmem_analysis.c      # Análisis de VSZ, RSS, Demand Paging y /proc/self/maps
├── capturas/                # Evidencias de ejecución
└── README.md
