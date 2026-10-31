/*
 * seri_toplama.c - 0'dan N-1'e kadar olan tam sayıların tek iş parçacığıyla toplanması.
 *
 * Derleme : gcc -O2 -fopenmp -o seri_toplama seri_toplama.c
 * Çalıştırma: ./seri_toplama   (Windows cmd: seri_toplama.exe)
 *
 * Not: Döngü seri çalışır; OpenMP yalnızca duvar saati süresini ölçen
 * omp_get_wtime() işlevi için kullanılır. clock() Linux'ta işlemci
 * zamanını ölçtüğünden paralel programlarla karşılaştırmada yanıltıcıdır.
 * N değeri işlemcinize göre küçültülebilir (örneğin 1000000000LL).
 */
#include <stdio.h>
#include <omp.h>

#define N 4000000000LL

int main(void) {
    double sum = 0.0;
    double baslangic = omp_get_wtime();

    for (long long i = 0; i < N; i++) {
        sum += (double)i;
    }

    double sure = omp_get_wtime() - baslangic;

    /* sum bir double olduğundan %d değil %.0f biçimlendiricisi kullanılır. */
    printf("Seri Toplam: %.0f, Sure: %f saniye\n", sum, sure);
    return 0;
}
