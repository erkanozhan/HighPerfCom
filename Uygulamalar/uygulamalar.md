# Yüksek Başarımlı Hesaplama Uygulamaları

Bu belge, derste kullanılan paralelleştirme uygulamalarını bir araya getirir. Uygulamalar donanıma en yakın düzey olan çok çekirdekli programlamayla (OpenMP) başlar; ağ üzerindeki makinelere görev dağıtan Weka uygulamaları ise ayrı belgelerde anlatılır (bkz. Bölüm 2). Kuramsal arka plan için [Ders_Notu.md](../Ders_Notu.md) dosyasına, özellikle Bölüm 4 ve Bölüm 10'a bakınız.

Tüm kaynak kodlar `Uygulamalar/OpenMP/` dizinindedir.

---

## Bölüm 1: OpenMP ile Paylaşımlı Bellek Programlama

OpenMP (Open Multi-Processing), paylaşımlı bellekli sistemlerde (çok çekirdekli işlemciler) C, C++ ve Fortran programlarını çoklu iş parçacığıyla (multithreading) paralelleştirmek için kullanılan endüstri standardı bir uygulama programlama arayüzüdür (API).

### 1.1 Kurulum (Windows için MSYS2 ve GCC)

Windows ortamında OpenMP destekli C kodlarını derlemek için GCC derleyicisi gerekir.

1. **MSYS2 kurulumu:** [MSYS2 resmî web sitesinden](https://www.msys2.org/) kurulum dosyasını indirip kurun.
2. **Paketlerin güncellenmesi:** MSYS2 konsolunu açın ve şu komutu çalıştırın. Komut konsolun kapanmasını isterse konsolu yeniden açıp aynı komutu bir kez daha çalıştırın.
   ```bash
   pacman -Syu
   ```
3. **GCC kurulumu:**
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```
   Bu paket MINGW64 ortamına kurulur. MSYS2'nin önerdiği UCRT64 ortamını kullanıyorsanız `mingw-w64-ucrt-x86_64-gcc` paketini kurun ve aşağıdaki yolda `mingw64` yerine `ucrt64` yazın.
4. **Ortam değişkeni:** Windows "Ortam Değişkenleri" ayarlarında `Path` değişkenine GCC'nin bulunduğu dizini ekleyin (örneğin `C:\msys64\mingw64\bin`).
5. **Doğrulama:** Yeni bir komut istemi (`cmd`) açıp şu komutu çalıştırın; sürüm bilgisi görüntüleniyorsa kurulum başarılıdır:
   ```bash
   gcc --version
   ```

Linux'ta dağıtımın `gcc` paketi OpenMP desteğiyle gelir (örneğin Ubuntu'da `sudo apt install gcc`).

### 1.2 Derleme ve Çalıştırma

Kaynak kodu OpenMP desteğiyle derlemek için `-fopenmp` seçeneği kullanılır; `-O2` eniyileme (optimization) düzeyini açar:

```bash
# Derleme
gcc -O2 -fopenmp -o program_adi kaynak_kod.c

# Çalıştırma (Windows cmd)
program_adi.exe
# Çalıştırma (Linux, MSYS2 veya Git Bash)
./program_adi
```

İş parçacığı sayısı `OMP_NUM_THREADS` ortam değişkeniyle değiştirilebilir (cmd: `set OMP_NUM_THREADS=4`, bash: `export OMP_NUM_THREADS=4`).

**Ölçüm ortamı.** Aşağıdaki tüm çıktılar Intel Core i7-4790K (4 fiziksel çekirdek, Hyper-Threading ile 8 mantıksal çekirdek, 4.0 GHz), 16 GB bellek, Windows 11 ve MSYS2 GCC 14.2 ile `gcc -O2 -fopenmp` kullanılarak alınmıştır. Süreler makinenin o anki yüküne göre birkaç on yüzde değişebilir.

**Süre ölçümü üzerine not.** Paralel programlarda süre `omp_get_wtime()` ile ölçülmelidir; bu işlev duvar saati süresini (wall-clock time) verir. C kitaplığındaki `clock()` işlevi Linux'ta tüm iş parçacıklarının harcadığı toplam **işlemci zamanını** döndürür; bu nedenle paralel program hiç hızlanmamış, hatta yavaşlamış gibi görünür. (Windows'taki C kitaplığında `clock()` duvar saati süresi verdiği için bu hata Windows'ta fark edilmeyebilir.)

---

### 1.3 Örnek Uygulamalar

#### Örnek 1: İlk Paralel Bölge — `merhaba.c`

OpenMP'nin iş parçacığı takımı oluşturma mantığını gösterir.

```c
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
```

Ölçülen çıktı (satırların sırası her çalıştırmada değişir; bu, iş parçacıklarının eşzamanlı çalıştığını gösterir):

```text
Paralel calisma simdi basliyor!
Is parcacigi 1 / 8 calismaya basladi (0.001000 s sonra)
Is parcacigi 7 / 8 calismaya basladi (0.001000 s sonra)
Is parcacigi 2 / 8 calismaya basladi (0.001000 s sonra)
...
Is parcacigi 5 / 8 calismaya basladi (0.001000 s sonra)
```

- `#include <omp.h>`: OpenMP işlevleri için gerekli başlık dosyası.
- `#pragma omp parallel`: Ardından gelen bloğun takımdaki tüm iş parçacıklarınca yürütülmesini sağlar.
- `omp_get_thread_num()`: İş parçacığının takım içindeki kimliğini (0'dan başlar) döndürür.
- `omp_get_wtime()`: Duvar saati süresini saniye cinsinden verir. Bu platformda çözünürlüğü yaklaşık 1 ms'dir; bu yüzden tüm satırlarda aynı değer görülür.

#### Örnek 2: Seri ve Paralel Toplama — `seri_toplama.c`, `parallel_toplama.c`

$0 + 1 + \dots + (N-1)$ toplamı $N = 4 \times 10^9$ için önce tek iş parçacığıyla, sonra dört iş parçacığıyla hesaplanır.

**`seri_toplama.c`**

```c
#include <stdio.h>
#include <omp.h>

#define N 4000000000LL

int main(void) {
    double sum = 0.0;
    double baslangic = omp_get_wtime();

    for (long long i = 0; i < N; i++) {
        sum += (double)i;
    }

    double sure = omp_get_wtime() - baslangic;

    /* sum bir double olduğundan %d değil %.0f biçimlendiricisi kullanılır. */
    printf("Seri Toplam: %.0f, Sure: %f saniye\n", sum, sure);
    return 0;
}
```

**`parallel_toplama.c`**

```c
#include <stdio.h>
#include <omp.h>

#define N 4000000000LL

int main(void) {
    double sum = 0.0;
    double baslangic = omp_get_wtime();   /* duvar saati süresi */

    omp_set_num_threads(4);

    /* reduction(+:sum): her iş parçacığı 0 ile başlayan özel bir kopya
       kullanır; döngü sonunda kopyalar güvenli biçimde toplanır. */
    #pragma omp parallel for reduction(+:sum)
    for (long long i = 0; i < N; i++) {
        sum += (double)i;
    }

    double sure = omp_get_wtime() - baslangic;

    printf("Paralel Toplam: %.0f, Sure: %f saniye\n", sum, sure);
    return 0;
}
```

Ölçülen çıktılar:

```text
Seri Toplam: 7999999996067115008, Sure: 4.149000 saniye
Paralel Toplam: 7999999996075354112, Sure: 1.256000 saniye
```

Dört iş parçacığıyla hızlanma $4.149 / 1.256 \approx 3.3$'tür. Dört kat hızlanma kuramsal üst sınırdır; seri sürümde çok uzun süren bir hesap paralel sürümde en fazla bu oranda kısalır.

**Biçimlendirici hatası.** `double` türündeki bir değişkeni `printf` ile `%d` biçimlendiricisiyle yazdırmak tanımsız davranıştır. Kodun önceki sürümünde kullanılan `%11d` bu makinede ekrana `1729375548` gibi anlamsız bir sayı basıyordu; `gcc -Wall` bu hatayı `-Wformat` uyarısıyla bildirir. Doğrusu `%.0f`'tir.

**Seri ve paralel sonuçlar neden farklı?** Kesin sonuç $N(N-1)/2 = 7\,999\,999\,998\,000\,000\,000$'dır; iki program da bu değerden ve birbirinden farklı sonuç vermiştir. Bunun nedeni kayan noktalı aritmetikte toplamanın **birleşme özelliğinin** (associativity) olmamasıdır:

- **Seri toplamada** sayılar tek bir sırayla toplanır: $((a + b) + c) + d \dots$
- **Paralel toplamada** her iş parçacığı kendi aralığının toplamını hesaplar, en sonda bu alt toplamlar birleştirilir: $(a + b) + (c + d) \dots$

`double` türü 53 bitlik anlamlı kısma sahiptir ve tam sayıları ancak $2^{53} \approx 9 \times 10^{15}$'e kadar hatasız tutabilir. Toplam yaklaşık $8 \times 10^{18}$ olduğundan her toplamada yuvarlama yapılır; toplama sırası değiştiğinde yuvarlama hataları farklı birikir.

**Yalnızca tam sayılar toplansaydı fark olur muydu?** Toplamı tutan değişken tam sayı türünde (örneğin `long long`) olsaydı ve taşma (overflow) olmasaydı, `reduction` kullanan paralel kod seri kodla **tam olarak aynı** sonucu verirdi; çünkü tam sayı toplaması birleşme özelliğine sahiptir. Örnek 6'daki `yaris.c` programı bunu gösterir.

#### Örnek 3: Pi Sayısının Sayısal İntegralle Hesaplanması — `pi_hesaplama.c`

$\pi = \int_0^1 \frac{4}{1+x^2}\,dx$ integrali orta nokta kuralıyla $10^9$ adımda hesaplanır. `private(x)` her iş parçacığına kendi `x` kopyasını, `reduction(+:sum)` ise toplamın güvenli birleştirilmesini sağlar.

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

Ölçülen çıktı (8 iş parçacığı):

```text
Pi degeri: 3.141592653589769, 1000000000 adimda hesaplandi.
Hesaplama suresi: 1.179000 saniye
```

#### Örnek 4: Büyük Vektörlerde Toplama — `vektor_toplama.c`

$5 \times 10^7$ elemanlı iki vektörün toplamı veri paralelliğiyle hesaplanır; dizilerin ilk değer ataması da paralel yapılır. Kodun tamamı `vektor_toplama.c` dosyasındadır; temel kısım şudur:

```c
#pragma omp parallel for
for (int i = 0; i < N; i++) {
    C[i] = A[i] + B[i];
}
```

Ölçülen çıktı:

```text
Vektor toplama (50000000 eleman) islemi tamamlandi.
Hesaplama suresi: 0.205000 saniye
Ornek sonuc: C[100] = 300.0
```

Her eleman için yalnızca bir toplama yapılırken üç dizi (yaklaşık 1.2 GB) bellekten okunup yazılır. İşlem bellek sınırlıdır (bkz. Ders Notu, Bölüm 8); çekirdek sayısını artırmak bellek bant genişliği dolduktan sonra süreyi kısaltmaz.

#### Örnek 5: Hızlanma ve Verimlilik Ölçümü — `hizlanma.c`

Pi integralini $4 \times 10^8$ adımda 1, 2, 4 ve 8 iş parçacığıyla hesaplar; hızlanmayı $S(p)$, verimliliği $E(p)$ ve Karp–Flatt metriğini $e(p)$ yazdırır. Her ölçüm üç kez tekrarlanır ve en kısa süre alınır.

```c
#pragma omp parallel for num_threads(p) reduction(+:sum)
for (long i = 0; i < ADIM; i++) {
    double x = (i + 0.5) * step;   /* blok içinde tanımlı olduğu için özel */
    sum += 4.0 / (1.0 + x * x);
}
```

Ölçülen çıktı:

```text
   p     T(p) s     S(p)     E(p)       e(p)                 pi
   1      1.779     1.00     1.00          -  3.141592653589552
   2      1.036     1.72     0.86      0.165  3.141592653590022
   4      0.623     2.86     0.71      0.134  3.141592653590041
   8      0.396     4.49     0.56      0.112  3.141592653589885
```

Sonuçların yorumu Ders Notu Bölüm 4.5'tedir. İş parçacığı sayısı değiştikçe $\pi$'nin son basamaklarının değişmesi, yine toplama sırasının değişmesinden kaynaklanır.

#### Örnek 6: Yarış Durumu ve Koruma Yöntemleri — `yaris.c`

Aynı tam sayı toplamı ($N = 2 \times 10^7$) korumasız, `atomic`, `critical` ve `reduction` ile hesaplanır.

```c
/* Korumasız: okuma-değiştirme-yazma adımları iç içe geçer, güncellemeler kaybolur */
#pragma omp parallel for
for (long long i = 0; i < N; i++) {
    toplam += i;
}

/* atomic: her güncelleme bölünmez donanım komutuyla yapılır */
#pragma omp parallel for
for (long long i = 0; i < N; i++) {
    #pragma omp atomic
    toplam += i;
}
```

Ölçülen çıktı (8 iş parçacığı):

```text
Is parcacigi sayisi: 8
Beklenen toplam    : 199999990000000

Yontem                        Sonuc   Sure (s) Dogru mu?
korumasiz            68749997500000      0.018 HAYIR
atomic              199999990000000      0.463 evet
critical            199999990000000      1.142 evet
reduction           199999990000000      0.004 evet
```

Korumasız sürüm ikinci bir çalıştırmada `9374998750000` sonucunu vermiştir: Yarış durumunda sonuç her çalıştırmada değişebilir. Tam sayı toplamı kullanıldığı için `reduction` sonucu beklenen değerle birebir aynıdır. Ayrıntılı yorum Ders Notu Bölüm 10.7'dedir.

#### Örnek 7: Zamanlama Türlerinin Karşılaştırılması — `zamanlama.c`

$i$'inci iterasyonun maliyeti $i$ ile orantılıdır (üçgen iş yükü). Aynı döngü farklı `schedule` maddeleriyle çalıştırılır. `schedule(runtime)` satırı `OMP_SCHEDULE` ortam değişkenini okur:

```bash
gcc -O2 -fopenmp -o zamanlama zamanlama.c -lm
OMP_NUM_THREADS=4 ./zamanlama
OMP_SCHEDULE="dynamic,16" ./zamanlama
```

Ölçülen çıktı (4 iş parçacığı):

```text
Is parcacigi sayisi: 4
schedule                 Sure (s)
seri                        6.967   (kontrol: 0.000018)
static (varsayilan)         3.591   (kontrol: 0.000018)
static,1                    2.291   (kontrol: 0.000018)
dynamic,1                   2.057   (kontrol: 0.000018)
dynamic,64                  1.943   (kontrol: 0.000018)
guided                      2.085   (kontrol: 0.000018)
runtime (OMP_SCHEDULE)      1.904   (kontrol: 0.000018)
```

"kontrol" sütunu tüm sürümlerin aynı sonucu hesapladığını doğrulamak içindir. Yorum için Ders Notu Bölüm 10.8'e bakınız.

#### Örnek 8: Veri Paylaşım Maddeleri ve Yanlış Paylaşım — `paylasim.c`

Birinci bölüm `private`, `firstprivate` ve `lastprivate` farkını; ikinci bölüm aynı önbellek satırını paylaşan sayaçların yarattığı yanlış paylaşımı (false sharing) gösterir.

Ölçülen çıktı (8 iş parçacığı):

```text
Bolum 1: a = 10 (degismez), b = 10 (degismez), c = 49 (son iterasyon: 49)

Bolum 2 (8 is parcacigi, her biri 100000000 artirma):
  bitisik sayaclar (ayni onbellek satiri): 0.561 s
  dolgulu sayaclar (ayri onbellek satiri): 0.333 s
  oran: 1.7 kat
```

`OMP_NUM_THREADS=4` ile oran 1.5 (0.421 s ve 0.274 s) bulunmuştur. Yorum için Ders Notu Bölüm 10.5 ve 10.10'a bakınız.

#### Önerilen Alıştırmalar

1. `hizlanma.c` programını kendi makinenizde çalıştırın; $S(p)$ eğrisini Amdahl Yasası'ndaki eğrilerle karşılaştırın ve Karp–Flatt değerinden seri payı kestirin.
2. `zamanlama.c` programında `OMP_SCHEDULE` değişkenine farklı blok boyutları (1, 16, 256, 4096) vererek blok boyutu ile süre arasındaki ilişkiyi çizin.
3. `yaris.c` programını `-O0` ve `-O2` ile ayrı ayrı derleyip korumasız sürümün sonuçlarını karşılaştırın.
4. `paylasim.c` programındaki dolgu (`dolgu`) dizisini kaldırıp yapının boyutunu 8 bayta indirin ve sürenin nasıl değiştiğini gözlemleyin.

---

## Bölüm 2: Weka ile Dağıtık İşleme

OpenMP tek bir makinenin çekirdeklerini kullanır. Veri boyutu ya da görev sayısı arttığında ağa bağlı birden fazla bilgisayar kullanılabilir. Bu konu, tekrarı önlemek için ayrı belgelerde anlatılmıştır:

- [weka_server_setup.md](../weka_server_setup.md): `wekaServer` paketiyle bir ana sunucu (master) ve ona bağlı yardımcı sunucuların (slave) kurulması ve görevlerin bu sunuculara gönderilmesi. WekaServer veriyi bölmez; her görevin tamamını yükü en az olan sunucuya atar.
- [weka_dagitik_spark_giris.md](../weka_dagitik_spark_giris.md), [weka_spark_kullanimi.md](../weka_spark_kullanimi.md) ve [weka_knowledgeflow_spark.md](../weka_knowledgeflow_spark.md): `distributedWekaSpark` paketiyle verinin Apache Spark üzerinde bölümlere ayrılarak paralel işlenmesi.
