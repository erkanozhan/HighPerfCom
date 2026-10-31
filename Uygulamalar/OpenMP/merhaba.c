/*
 * merhaba.c - OpenMP ile ilk paralel bölge.
 *
 * Derleme : gcc -O2 -fopenmp -o merhaba merhaba.c
 * Çalıştırma: ./merhaba   (Windows cmd: merhaba.exe)
 * İş parçacığı sayısı OMP_NUM_THREADS ortam değişkeniyle değiştirilebilir.
 */
#include <stdio.h>
#include <omp.h>

int main(void) {
    printf("Paralel calisma simdi basliyor!\n");
    double baslangic = omp_get_wtime();

    #pragma omp parallel
    {
        int id = omp_get_thread_num();      /* bu iş parçacığının kimliği */
        int takim = omp_get_num_threads();  /* takımdaki iş parçacığı sayısı */
        /* omp_get_wtime() mutlak bir zaman değil, keyfi bir başlangıçtan
           itibaren geçen saniyedir; anlamlı olan iki ölçüm arasındaki farktır. */
        double gecen = omp_get_wtime() - baslangic;
        printf("Is parcacigi %d / %d calismaya basladi (%.6f s sonra)\n",
               id, takim, gecen);
    }

    return 0;
}
