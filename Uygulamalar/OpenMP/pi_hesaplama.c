#include <stdio.h>
#include <omp.h>

static long num_steps = 1000000000;
double step;

int main () {
    int i;
    double x, pi, sum = 0.0;
    double start_time, run_time;

    step = 1.0 / (double) num_steps;
    
    start_time = omp_get_wtime();

    // Paralel for döngüsü ve reduction kullanımı
    #pragma omp parallel for private(x) reduction(+:sum)
    for (i = 0; i < num_steps; i++) {
        x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0 + x * x);
    }
    
    pi = step * sum;
    run_time = omp_get_wtime() - start_time;

    printf("Pi degeri: %.15f, %ld adimda hesaplandi.\n", pi, num_steps);
    printf("Hesaplama suresi: %f saniye\n", run_time);
    
    return 0;
}
