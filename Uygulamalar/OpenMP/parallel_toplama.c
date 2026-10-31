/*
 * parallel_toplama.c - 0'dan N-1'e kadar olan tam sayıların OpenMP reduction ile toplanması.
 *
 * Derleme : gcc -O2 -fopenmp -o parallel_toplama parallel_toplama.c
 * Çalıştırma: ./parallel_toplama   (Windows cmd: parallel_toplama.exe)
 *
 * N değeri işlemcinize göre küçültülebilir (örneğin 1000000000LL).
 * Dört iş parçacığıyla elde edilebilecek hızlanma en fazla 4 kattır.
 */
#include <stdio.h>
#include <omp.h>

#define N 4000000000LL

int main(void) {
    double sum = 0.0;
    double baslangic = omp_get_wtime();   /* duvar saati süresi */

    omp_set_num_threads(4);

    /* reduction(+:sum): her iş parçacığı 0 ile başlayan özel bir kopya
       kullanır; döngü sonunda kopyalar güvenli biçimde toplanır. */
    #pragma omp parallel for reduction(+:sum)
    for (long long i = 0; i < N; i++) {
        sum += (double)i;
    }

    double sure = omp_get_wtime() - baslangic;

    printf("Paralel Toplam: %.0f, Sure: %f saniye\n", sum, sure);
    return 0;
}
