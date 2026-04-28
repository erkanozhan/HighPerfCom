# Yüksek Başarımlı Hesaplama Uygulamaları

Bu belge, Yüksek Başarımlı Hesaplama (HPC) kapsamında kullanılan temel paralelleştirme ve dağıtık işlem uygulamalarını tek bir çatı altında toplamaktadır. Uygulamalar mantıksal olarak donanım seviyesine en yakın olan çok çekirdekli programlama (OpenMP) ile başlayıp, ağ tabanlı dağıtık veri işleme mimarisi (Weka) ile devam etmektedir.

---

## Bölüm 1: OpenMP ile Paylaşımlı Bellek Paralel Programlama

OpenMP, paylaşımlı bellek mimarilerinde (çok çekirdekli işlemciler) çoklu iş parçacığı (multithreading) kullanarak C, C++ ve Fortran programlarını paralelleştirmek için kullanılan endüstri standardı bir API'dir.

### 1.1 Kurulum Adımları (Windows için MSYS2 ve GCC)

Windows ortamında OpenMP destekli C kodlarını derleyebilmek için GCC derleyicisine ihtiyaç vardır.

1. **MSYS2 Kurulumu:** 
   [MSYS2 resmi web sitesinden](https://www.msys2.org/) kurulum dosyasını indirin ve kurun.
2. **Paketleri Güncelleme:** 
   Kurulumdan sonra MSYS2 konsolunu açıp şu komutu çalıştırarak sistem paketlerini güncelleyin:
   ```bash
   pacman -Syu
   ```
3. **GCC Derleyicisinin Kurulumu:**
   Aşağıdaki komut ile C/C++ derleyicisini kurun:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```
4. **Ortam Değişkenlerinin Ayarlanması:**
   Windows "Ortam Değişkenleri" ayarlarında `Path` değişkenine GCC'nin kurulu olduğu dizini ekleyin (Örn: `C:\msys64\mingw64\bin`).
5. **Doğrulama:**
   Komut istemcisini (`cmd`) açıp aşağıdaki komutu girdiğinizde versiyon bilgisi geliyorsa kurulum başarılıdır:
   ```bash
   gcc --version
   ```

### 1.2 Kodları Derleme ve Çalıştırma Mantığı

Yazdığınız `.c` uzantılı kaynak kodları OpenMP kütüphanesi ile derlemek için `-fopenmp` parametresini kullanmanız gerekir:

```bash
# Derleme Komutu
gcc -fopenmp -o program_adi kaynak_kod.c

# Çalıştırma Komutu (Windows)
.\program_adi.exe
```

---

### 1.3 Örnek Uygulamalar

#### Örnek 1.3.1: Paralel Çalışma (Hello World)
Bu örnek, OpenMP'nin çoklu iş parçacığı oluşturma mantığını gösterir.

**`openmp_hello.c`**
```c
#include <stdio.h>
#include <omp.h>

int main() {
    printf("Paralel calisma simdi basliyor!\n");

    // OpenMP ile paralel bir blok başlatılır.
    #pragma omp parallel
    {
        // Her bir thread kendi kimliğini (ID) alır.
        int thread_id = omp_get_thread_num();
        
        // Her bir thread kendi çalışma zamanını alır.
        double omp_time = omp_get_wtime();
        
        // Her thread ekrana kendi ID'sini ve çalışma anını yazdırır.
        printf("Thread %d calismaya basladi. OMP time: %f seconds\n", thread_id, omp_time);
    }

    return 0;
}
```

#### Örnek 1.3.2: Performans Karşılaştırması (Seri vs Paralel Toplama)
Bu örneklerde, milyarlarca iterasyon içeren bir döngünün tek çekirdekle (seri) ve çok çekirdekle (paralel) işlenmesi arasındaki performans farkı ölçülmektedir.

**Seri Toplama (`seri_toplam.c`):**
```c
#include <stdio.h>
#include <time.h>
#define N 4000000000LL // İşlemci gücüne göre bu değer değiştirilebilir

int main() {
    double sum = 0;
    clock_t start = clock();

    // Döngü tek bir thread ile sırayla çalıştırılır.
    for (long long int i = 0; i < N; i++) {
        sum += i;
    }

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Seri Toplam: %11.0f, Süre: %f saniye\n", sum, time_spent);
    return 0;
}
```

**Paralel Toplama (`paralel_toplam.c`):**
```c
#include <stdio.h>
#include <omp.h>
#include <time.h>
#define N 4000000000LL

int main() {
    double sum = 0;
    clock_t start = clock();

    // Kullanılacak thread sayısı manuel olarak 4'e ayarlanıyor.
    omp_set_num_threads(4);

    // Paralel for döngüsü: İş yükü 4 thread'e bölünür.
    // reduction(+:sum) parametresi, her thread'in kendi yerel toplamını 
    // tutmasını ve en sonunda güvenli bir şekilde birleştirmesini (race condition engelleme) sağlar.
    #pragma omp parallel for reduction(+:sum)
    for (long long int i = 0; i < N; i++) {
        sum += i;
    }

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Paralel Toplam: %11.0f, Süre: %f saniye\n", sum, time_spent);
    return 0;
}
```

#### Örnek 1.3.3: Pi Sayısı Hesaplama (Sayısal İntegrasyon)
Bu örnek, OpenMP kullanarak `reduction` parametresi ile Pi sayısının sayısal integrasyon yöntemiyle nasıl paralel hesaplanabileceğini göstermektedir. Hem değişkenlerin yerel (`private`) kullanımlarını hem de genel (`reduction`) işlemlerini içerir.

**`pi_hesaplama.c`:**
```c
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
```

#### Örnek 1.3.4: Büyük Veri Setlerinde Vektör Toplama
Bu örnek, çok büyük veri dizileri (vektörler) üzerinde paralelleştirmenin veri paralelliği (data parallelism) kullanılarak nasıl uygulanacağını gösterir. Aynı zamanda dizi ilklendirme işleminin de (initialization) nasıl paralel yapılabileceği örneklenmiştir.

**`vektor_toplama.c`:**
```c
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
```

---

## Bölüm 2: Weka ile Dağıtık Veri İşleme (Master-Slave Mimarisi)

OpenMP tek bir makinenin çekirdeklerini kullanırken, veri boyutları ve görev karmaşıklığı çok büyüdüğünde ağa bağlı birden fazla bilgisayarı kullanmak (Dağıtık Hesaplama) gerekir. Weka'nın Dağıtık İşlem (WekaServer) özelliği, büyük veri kümeleri üzerindeki makine öğrenmesi analizlerini bir **Master (Ana)** ve ona bağlı birçok **Slave (İşçi)** sunucuya dağıtarak işlem sürelerini dramatik olarak kısaltır.

### 2.1 Önkoşullar

Tüm Master ve Slave makinelerde aşağıdaki şartlar sağlanmalıdır:

1. **Java Runtime Environment (JRE) Sürüm 8 (1.8):** 
   * Weka, Java 8 ile en kararlı şekilde çalışmaktadır. Java 8 sürümünün kurulu olduğundan emin olun.
   * `java -version` komutu ile versiyonu teyit edebilirsiniz.
   * Sisteminizde birden fazla Java varsa `JAVA_HOME` ve `Path` çevre değişkenlerini ayarlayarak Java 8'i öncelikli yapın.
2. **Weka JAR Dosyası:**
   * Weka'nın kurulu olduğu dizindeki ana `weka.jar` dosyası (Örn: `C:\Program Files\Weka-3-8-6\weka.jar`) hem Master hem de tüm Slave bilgisayarlarda bulunmalıdır.
3. **Güvenlik Duvarı (Firewall) Ayarları:**
   * Makinelerin ağ üzerinden haberleşebilmesi için kullanılacak portlara (örneğin 8085) Windows Defender veya Linux `ufw` üzerinden gelen/giden (inbound/outbound) kuralları ile izin verilmelidir.

### 2.2 Master Server'ı Başlatma

Master sunucu; hesaplama görevlerini kabul eden, bu görevleri parçalara ayıran ve boşta olan Slave makinelere dağıtıp onlardan gelen sonuçları birleştiren birimdir. 

* **IP Tespiti:** Komut istemcisinde `ipconfig` (Windows) veya `ifconfig` (Linux) yazarak Master makinenin IPv4 adresini öğrenin.

Master'ı başlatmak için komut satırı (cmd) üzerinden şu komutu çalıştırın:

```bash
# Genel Yapı:
# java -cp <WEKA_JAR_YOLU> -Xmx<MAKS_BELLEK> -Djava.awt.headless=true weka.Run WekaServer -host <MASTER_IP> -port <PORT> -load-adjust <YUK_AYARI> -slots <SLOT> -staleTime <ZAMAN_ASIMI>

# Örnek Çalıştırma Komutu (Windows, 8GB RAM ataması, IP: 10.222.18.190):
java -classpath "C:\Program Files\Weka-3-8-6\weka.jar" -Xmx8G -Djava.awt.headless=true weka.Run WekaServer -host 10.222.18.190 -port 8085 -load-adjust 4 -slots 1 -staleTime -1
```

**Parametrelerin Anlamları:**
* `-classpath` veya `-cp`: Weka JAR dosyasının tam yolu.
* `-Xmx8G`: Master sunucuya tahsis edilen maksimum RAM (Burada 8 Gigabayt).
* `-Djava.awt.headless=true`: Grafik arayüzü (GUI) olmadan arka planda çalışmasını sağlar.
* `-host`: Master'ın IP adresi (Sadece kendi üzerinde deneme yapacaksanız `localhost` yazabilirsiniz).
* `-port`: İletişim için dinlenecek ağ portu.
* `-load-adjust`: Yük dengeleme ayarı (genelde 4 önerilir).
* `-slots`: Aynı anda alınabilecek eşzamanlı görev kapasitesi.
* `-staleTime -1`: Görev zaman aşımı olmaması anlamına gelir. Eğer buraya örneğin 20 (dakika) girerseniz ve bir Slave'in analizi bitirmesi 45 dakika sürerse Master sunucu zaman aşımı sebebiyle kapanacak ve tüm hesaplama süreci boşa gidecektir.

### 2.3 Slave Makineleri (İşçileri) Başlatma ve Master'a Bağlama

Slave makineler, ağ üzerinden Master sunucunun gösterdiği işlemleri gerçekleştiren amele/işçi sunuculardır. Her bir Slave makinesinin komut satırında kendi IP'si ve bağlanacağı Master'ın IP'si açıkça belirtilmelidir.

Slave makine üzerinde komut istemcisini (cmd) açarak şu komutu çalıştırın:

```bash
# Genel Yapı:
# java -cp <WEKA_JAR_YOLU> -Xmx<MAKS_BELLEK> -Djava.awt.headless=true weka.Run WekaServer -host <SLAVE_IP> -port <SLAVE_PORT> -master <MASTER_IP>:<MASTER_PORT> -slots <SLOT> -staleTime -1

# Örnek Çalıştırma Komutu (Windows, 6GB RAM, 4 Görev Yuvası, Master'a Bağlanma):
java -Djava.awt.headless=true -classpath "C:\Program Files\Weka-3-8-6\weka.jar" -Xmx6G weka.Run WekaServer -host 10.202.17.35 -port 8085 -master 10.202.16.171:8085 -slots 4 -staleTime -1
```

**Önemli Ayrıntılar:**
* `-host`: O an komutu yazdığınız **Slave (İşçi)** bilgisayarın kendi IP adresi.
* `-master`: Görevleri alacağı **Master (Ana)** bilgisayarın IP adresi ve Portu (Örn: `10.202.16.171:8085`).
* `-slots 4`: Bu Slave makinesinin donanımı iyiyse, aynı anda 4 farklı analizi işleyebileceğini belirtir. Çekirdek ve RAM durumuna göre artırılıp azaltılabilir.

Bu komutlar doğru şekilde çalıştırıldığında, Master makine ağı dinleyecek, Slave makineler de Master'a bağlanarak komut beklemeye başlayacaktır. İş yükü Master üzerinden sisteme gönderildiğinde, parçalanan veri kümeleri ağ üzerinden Slave'lere aktarılır, paralelde işlenir ve toplanan sonuçlar tekrar Master'da birleştirilir.