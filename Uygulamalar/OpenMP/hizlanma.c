/*
 * hizlanma.c - Pi integralinin 1, 2, 4 ve 8 iş parçacığıyla hesaplanması;
 * hızlanma S(p), verimlilik E(p) ve Karp-Flatt seri kesir kestirimi e(p).
 *
 *   S(p) = T(1) / T(p)
 *   E(p) = S(p) / p
 *   e(p) = (1/S(p) - 1/p) / (1 - 1/p)    (Karp-Flatt metriği)
 *
 * Her ölçüm 3 kez tekrarlanır ve en kısa süre alınır.
 *
 * Derleme : gcc -O2 -fopenmp -o hizlanma hizlanma.c
 * Çalıştırma: ./hizlanma
 */
#include <stdio.h>
#include <omp.h>

#define ADIM 400000000L
#define TEKRAR 3

static double pi_hesapla(int p, double *pi_sonuc) {
    const double step = 1.0 / (double)ADIM;
    double en_iyi = 1e30;

    for (int r = 0; r < TEKRAR; r++) {
        double sum = 0.0;
        double t = omp_get_wtime();
        #pragma omp parallel for num_threads(p) reduction(+:sum)
        for (long i = 0; i < ADIM; i++) {
            double x = (i + 0.5) * step;   /* blok içinde tanımlı: özel */
            sum += 4.0 / (1.0 + x * x);
        }
        t = omp_get_wtime() - t;
        if (t < en_iyi) en_iyi = t;
        *pi_sonuc = step * sum;
    }
    return en_iyi;
}

int main(void) {
    int ip_sayilari[] = {1, 2, 4, 8};
    double t1 = 0.0, pi = 0.0;

    printf("%4s %10s %8s %8s %10s %18s\n", "p", "T(p) s", "S(p)", "E(p)", "e(p)", "pi");
    for (int k = 0; k < 4; k++) {
        int p = ip_sayilari[k];
        double t = pi_hesapla(p, &pi);
        if (p == 1) {
            t1 = t;
            printf("%4d %10.3f %8.2f %8.2f %10s %18.15f\n", p, t, 1.0, 1.0, "-", pi);
        } else {
            double S = t1 / t;
            double E = S / p;
            double e = (1.0 / S - 1.0 / p) / (1.0 - 1.0 / p);
            printf("%4d %10.3f %8.2f %8.2f %10.3f %18.15f\n", p, t, S, E, e, pi);
        }
    }
    return 0;
}
