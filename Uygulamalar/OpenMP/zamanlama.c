/*
 * zamanlama.c - Dengesiz iş yükünde schedule türlerinin karşılaştırılması.
 *
 * i'inci iterasyonun maliyeti i ile doğru orantılıdır (üçgen iş yükü):
 * son iterasyonlar ilk iterasyonlardan çok daha ağırdır. Varsayılan
 * statik bölmede son bloğu alan iş parçacığı en çok işi üstlenir.
 *
 * Derleme : gcc -O2 -fopenmp -o zamanlama zamanlama.c -lm
 * Çalıştırma: ./zamanlama
 * schedule(runtime) satırı OMP_SCHEDULE ortam değişkenini okur, örneğin:
 *   OMP_SCHEDULE="dynamic,16" ./zamanlama
 */
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define N 20000

static double is_yuku(int i) {
    double s = 0.0;
    for (int j = 0; j < i; j++) {
        s += sin((double)j) * 1e-9;
    }
    return s;
}

#define OLC(ETIKET, SCHED)                                              \
    do {                                                                \
        double toplam = 0.0;                                            \
        double t = omp_get_wtime();                                     \
        _Pragma(SCHED)                                                  \
        for (int i = 0; i < N; i++) toplam += is_yuku(i);               \
        t = omp_get_wtime() - t;                                        \
        printf("%-22s %10.3f   (kontrol: %.6f)\n", ETIKET, t, toplam);  \
    } while (0)

int main(void) {
    printf("Is parcacigi sayisi: %d\n", omp_get_max_threads());
    printf("%-22s %10s\n", "schedule", "Sure (s)");

    double t = omp_get_wtime();
    double seri = 0.0;
    for (int i = 0; i < N; i++) seri += is_yuku(i);
    t = omp_get_wtime() - t;
    printf("%-22s %10.3f   (kontrol: %.6f)\n", "seri", t, seri);

    OLC("static (varsayilan)", "omp parallel for schedule(static) reduction(+:toplam)");
    OLC("static,1",            "omp parallel for schedule(static,1) reduction(+:toplam)");
    OLC("dynamic,1",           "omp parallel for schedule(dynamic,1) reduction(+:toplam)");
    OLC("dynamic,64",          "omp parallel for schedule(dynamic,64) reduction(+:toplam)");
    OLC("guided",              "omp parallel for schedule(guided) reduction(+:toplam)");
    OLC("runtime (OMP_SCHEDULE)", "omp parallel for schedule(runtime) reduction(+:toplam)");
    return 0;
}
