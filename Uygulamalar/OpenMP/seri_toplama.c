#include <stdio.h>
#include <time.h>
# define N 4000000000LL // İşlemcinizin gücüne göre buradaki değeri düşürebilirsiniz genelde en baştaki rakamı daha düşük seçerek hesaplama süresini çok beklemeden görebilirsiniz
int main(){
    double sum=0;
    clock_t start=clock();
    for (long long int i=0; i<N;i++) {
        sum+=i;
    }
    clock_t end=clock();
    double time_spent=(double)(end-start)/CLOCKS_PER_SEC;
    printf("Seri Toplam: %11d, Süre: %f saniye\n", sum, time_spent);
    return 0;
}