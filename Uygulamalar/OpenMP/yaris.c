/*
 * yaris.c - Yarış durumu (race condition) ve dört koruma biçiminin karşılaştırılması.
 *
 * Aynı toplam (0 + 1 + ... + N-1) dört farklı yolla paralel hesaplanır:
 *   1) Korumasız   : paylaşılan değişkene eşzamanlı yazma -> yanlış sonuç
 *   2) atomic      : her güncelleme bölünmez donanım komutuyla yapılır
 *   3) critical    : her güncelleme kilitle korunan bölgede yapılır
 *   4) reduction   : her iş parçacığı özel kopyada toplar, sonda birleştirilir
 *
 * Derleme : gcc -O2 -fopenmp -o yaris yaris.c
 * Çalıştırma: ./yaris
 */
#include <stdio.h>
#include <omp.h>

#define N 20000000LL

int main(void) {
    const long long beklenen = N * (N - 1) / 2;
    long long toplam;
    double t;

    printf("Is parcacigi sayisi: %d\n", omp_get_max_threads());
    printf("Beklenen toplam    : %lld\n\n", beklenen);
    printf("%-12s %22s %10s %s\n", "Yontem", "Sonuc", "Sure (s)", "Dogru mu?");

    /* 1) Korumasız: okuma-değiştirme-yazma adımları iş parçacıkları
          arasında iç içe geçer; güncellemeler kaybolur. */
    toplam = 0;
    t = omp_get_wtime();
    #pragma omp parallel for
    for (long long i = 0; i < N; i++) {
        toplam += i;
    }
    t = omp_get_wtime() - t;
    printf("%-12s %22lld %10.3f %s\n", "korumasiz", toplam, t,
           toplam == beklenen ? "evet" : "HAYIR");

    /* 2) atomic */
    toplam = 0;
    t = omp_get_wtime();
    #pragma omp parallel for
    for (long long i = 0; i < N; i++) {
        #pragma omp atomic
        toplam += i;
    }
    t = omp_get_wtime() - t;
    printf("%-12s %22lld %10.3f %s\n", "atomic", toplam, t,
           toplam == beklenen ? "evet" : "HAYIR");

    /* 3) critical */
    toplam = 0;
    t = omp_get_wtime();
    #pragma omp parallel for
    for (long long i = 0; i < N; i++) {
        #pragma omp critical
        toplam += i;
    }
    t = omp_get_wtime() - t;
    printf("%-12s %22lld %10.3f %s\n", "critical", toplam, t,
           toplam == beklenen ? "evet" : "HAYIR");

    /* 4) reduction */
    toplam = 0;
    t = omp_get_wtime();
    #pragma omp parallel for reduction(+:toplam)
    for (long long i = 0; i < N; i++) {
        toplam += i;
    }
    t = omp_get_wtime() - t;
    printf("%-12s %22lld %10.3f %s\n", "reduction", toplam, t,
           toplam == beklenen ? "evet" : "HAYIR");

    return 0;
}
