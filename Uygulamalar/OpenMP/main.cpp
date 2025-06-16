#include <stdio.h>
#include <omp.h>

int main() {
    printf("Paralel calisma simdi basliyor!\n");

#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        double omp_time = omp_get_wtime();
        printf("Thread %d calismaya basladi OMP time: %f seconds\n", thread_id, omp_time);
    }

    return 0;
}
