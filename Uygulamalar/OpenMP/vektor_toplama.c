#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 50000000

int main() {
    double *A, *B, *C;
    
    // Bellek tahsisi
    A = (double*)malloc(N * sizeof(double));
    B = (double*)malloc(N * sizeof(double));
    C = (double*)malloc(N * sizeof(double));

    // Dizi başlatma işlemi (Paralel)
    #pragma omp parallel for
    for(int i = 0; i < N; i++) {
        A[i] = i * 1.0;
        B[i] = i * 2.0;
    }

    double start_time = omp_get_wtime();

    // Vektör Toplama İşlemi (Paralel)
    #pragma omp parallel for
    for(int i = 0; i < N; i++) {
        C[i] = A[i] + B[i];
    }

    double run_time = omp_get_wtime() - start_time;

    printf("Vektor toplama (%d eleman) islemi tamamlandi.\n", N);
    printf("Hesaplama suresi: %f saniye\n", run_time);
    printf("Ornek sonuc: C[100] = %.1f\n", C[100]);

    // Bellek temizliği
    free(A);
    free(B);
    free(C);
    
    return 0;
}
