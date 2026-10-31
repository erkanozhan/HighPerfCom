/*
 * paylasim.c - Veri paylaşım maddeleri ve yanlış paylaşım (false sharing).
 *
 * Bölüm 1: private, firstprivate ve lastprivate arasındaki fark.
 * Bölüm 2: Her iş parçacığının kendi sayacını artırdığı iki düzen:
 *   (a) sayaçlar bitişik dizide: aynı 64 baytlık önbellek satırını paylaşır
 *   (b) her sayaç kendi 64 baytlık satırında (dolgu / padding)
 * Mantıksal olarak hiçbir veri paylaşılmadığı halde (a) düzeni, önbellek
 * satırının çekirdekler arasında sürekli el değiştirmesi nedeniyle yavaştır.
 *
 * Derleme : gcc -O2 -fopenmp -o paylasim paylasim.c
 * Çalıştırma: ./paylasim
 */
#include <stdio.h>
#include <omp.h>

#define TEKRAR 100000000L
#define EN_FAZLA_IP 64

/* Her sayacı ayrı bir önbellek satırına yerleştiren yapı. */
typedef struct {
    volatile long deger;
    char dolgu[64 - sizeof(long)];
} DolguluSayac;

int main(void) {
    /* ---------------- Bölüm 1 ---------------- */
    int a = 10, b = 10, c = 10;

    #pragma omp parallel for num_threads(4) private(a) firstprivate(b) lastprivate(c)
    for (int i = 0; i < 8; i++) {
        a = i;        /* private: giriş değeri tanımsızdır, önce atanmalıdır */
        b += i;       /* firstprivate: her kopya 10 ile başlar */
        c = i * i;    /* lastprivate: son iterasyonun (i = 7) değeri dışarı çıkar */
    }
    printf("Bolum 1: a = %d (degismez), b = %d (degismez), c = %d (son iterasyon: 49)\n\n",
           a, b, c);

    /* ---------------- Bölüm 2 ---------------- */
    int p = omp_get_max_threads();
    if (p > EN_FAZLA_IP) p = EN_FAZLA_IP;

    static volatile long bitisik[EN_FAZLA_IP];
    static DolguluSayac dolgulu[EN_FAZLA_IP];

    double t = omp_get_wtime();
    #pragma omp parallel num_threads(p)
    {
        int id = omp_get_thread_num();
        for (long k = 0; k < TEKRAR; k++) bitisik[id]++;
    }
    double t_bitisik = omp_get_wtime() - t;

    t = omp_get_wtime();
    #pragma omp parallel num_threads(p)
    {
        int id = omp_get_thread_num();
        for (long k = 0; k < TEKRAR; k++) dolgulu[id].deger++;
    }
    double t_dolgulu = omp_get_wtime() - t;

    printf("Bolum 2 (%d is parcacigi, her biri %ld artirma):\n", p, TEKRAR);
    printf("  bitisik sayaclar (ayni onbellek satiri): %.3f s\n", t_bitisik);
    printf("  dolgulu sayaclar (ayri onbellek satiri): %.3f s\n", t_dolgulu);
    printf("  oran: %.1f kat\n", t_bitisik / t_dolgulu);
    return 0;
}
