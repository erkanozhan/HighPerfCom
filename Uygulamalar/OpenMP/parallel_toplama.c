# include <stdio.h>
# include <omp.h>
# include <time.h>
# define N 4000000000LL

                      /* İşlemcinizin gücüne göre buradaki değeri düşürebilirsiniz genelde en baştaki 4 rakamını daha düşük seçerek 
                      hesaplama süresini çok beklemeden görebilirsiniz.
                      Burada tabii kiparalel hesaplama yapıldığı için seriyi de belki hiç sonuç çıkarmayan hesaplamalar burada 
                      hemen sonuç verebilir.*/

int main(){
    double sum=0;
    clock_t start =clock();
    omp_set_num_threads(4);
    #pragma omp parallel for reduction(+:sum)
    for (long long int i=0;i<N;i++){
        sum+=i;
    }
clock_t end=clock();
double time_spent=(double)(end-start)/CLOCKS_PER_SEC;
printf("Paralel Toplam: %11d, Süre: %f saniye\n",sum,time_spent);
return 0;
}