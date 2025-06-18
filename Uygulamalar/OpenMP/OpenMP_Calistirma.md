# OpenMP Çalıştırma Adımları

## 1. MSYS2 Kurulumu
MSYS2'yi [resmi web sitesinden](https://www.msys2.org/) indirip kurulum sihirbazını takip ederek yükleyin. Kurulum tamamlandıktan sonra MSYS2 konsolunu açın ve başlangıç paketlerini güncellemek için aşağıdaki komutu çalıştırın:
```bash
pacman -Syu
```
Bu işlem, sistemin güncel paketlerle çalışmasını sağlar.

## 2. Paket Güncellemeleri
MSYS2 konsolunda aşağıdaki komutu çalıştırarak tüm paketleri güncelleyin:
```bash
pacman -Syu
```

## 3. GCC Derleyicisinin Kurulumu
GCC derleyicisini yüklemek için aşağıdaki komutu çalıştırın:
```bash
pacman -S mingw-w64-x86_64-gcc
```

## 4. Ortam Değişkenlerinin Ayarlanması
Windows ortam değişkenlerine GCC'nin yolunu ekleyin. Örneğin:
```
c:\msys64\mingw64\bin
```

## 5. GCC Kurulumunun Doğrulanması
GCC'nin doğru kurulduğunu doğrulamak için cmd ekranında aşağıdaki komutu çalıştırın:
```bash
gcc --version
```
Eğer GCC versiyon bilgisi görüntüleniyorsa kurulum başarılıdır. Aksi takdirde önceki adımları kontrol edin.

## 6. C Programı Yazma
Bir C programı oluşturun ve uygun bir isimle kaydedin. Örneğin:
```
openmp_deneme.c
```

## 7. Derleme ve Çalıştırma
`.c` dosyasının bulunduğu klasöre gidin ve aşağıdaki komutla programı derleyin:
```bash
gcc -fopenmp -o openmp_exe_adi openmp_deneme.c
```
Programı çalıştırmak için şu komutu kullanın:
```bash
.\openmp_exe_adi
```

# Örnek Programlar

### 1. Paralel ve Seri Çalışma Örneği

Aşağıda, OpenMP kullanılarak paralel çalışan basit bir C programı ve satır satır açıklamaları yer almaktadır:

```c
#include <stdio.h>
#include <omp.h>

int main() {
    // Paralel çalışma başlatılıyor.
    printf("Paralel calisma simdi basliyor!\n");

    // OpenMP ile paralel bir blok başlatılır.
    #pragma omp parallel
    {
        // Her bir thread kendi kimliğini alır.
        int thread_id = omp_get_thread_num();
        // Her bir thread kendi çalışma zamanını alır.
        double omp_time = omp_get_wtime();
        // Her bir thread ekrana kendi kimliği ve zaman bilgisini yazar.
        printf("Thread %d calismaya basladi OMP time: %f seconds\n", thread_id, omp_time);
    }

    return 0;
}
```
#### Seri Toplama Örneği
```c
#include <stdio.h>
#include <time.h>
# define N 4000000000LL // İşlemcinizin gücüne göre buradaki değeri düşürebilirsiniz genelde en baştaki rakamı daha düşük seçerek hesaplama süresini çok beklemeden görebilirsiniz
int main(){
    double sum=0;
    clock_t start=clock();
    // Bu döngüde, 0'dan N'e kadar olan tüm tamsayılar toplanmaktadır.
    // İşlem tamamen seri olarak, yani tek bir iş parçacığı (thread) ile gerçekleştirilir.
    for (long long int i=0; i<N;i++) {
        sum+=i;
    }
    clock_t end=clock();
    double time_spent=(double)(end-start)/CLOCKS_PER_SEC;
    // Hesaplanan toplam ve geçen süre ekrana yazdırılır.
    printf("Seri Toplam: %11d, Süre: %f saniye\n", sum, time_spent);
    return 0;
}
```

### Açıklamalar

- `#include <omp.h>`: OpenMP fonksiyonlarını kullanabilmek için gerekli başlık dosyasıdır.
- `#pragma omp parallel`: Bu direktif, kod bloğunun birden fazla thread tarafından paralel olarak çalıştırılmasını sağlar.
- `omp_get_thread_num()`: Her bir thread'in kendine özgü kimliğini (ID) döndürür.
- `omp_get_wtime()`: OpenMP tarafından sağlanan yüksek çözünürlüklü zaman bilgisini verir.
- Her thread, kendi kimliği ve zaman bilgisini ekrana yazarak paralel çalışmanın başladığını gösterir.

Bu örnek, OpenMP ile temel paralel programlamanın nasıl gerçekleştirileceğini göstermektedir.

### 2. Paralel Toplama Örneği
```c
# include <stdio.h>
# include <omp.h>
# include <time.h>
# define N 4000000000LL

                      /* İşlemcinizin gücüne göre buradaki değeri düşürebilirsiniz genelde en baştaki 4 rakamını daha düşük seçerek 
                      hesaplama süresini çok beklemeden görebilirsiniz.
                      Burada tabii kiparalel hesaplama yapıldığı için seriyi de belki hiç sonuç çıkarmayan hesaplamalar burada 
                      hemen sonuç verebilir.*/
int main(){
    double sum = 0;
    clock_t start = clock();

    // Kullanılacak thread sayısı 4 olarak ayarlanıyor.
    omp_set_num_threads(4);

    // Paralel for döngüsü başlatılıyor ve 'sum' değişkeni reduction ile toplanıyor.
    #pragma omp parallel for reduction(+:sum)
    for (long long int i = 0; i < N; i++){
        sum += i;
    }

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    // Sonuçlar ekrana yazdırılıyor: Toplam değer ve geçen süre.
    printf("Paralel Toplam: %11.0f, Süre: %f saniye\n", sum, time_spent);

    return 0;
}
```
