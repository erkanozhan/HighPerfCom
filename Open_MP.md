# İşlemci Mimarisi, Bellek Hiyerarşisi ve Veri Odaklı Tasarım Notları

Bu ders notları, modern bilgisayar mimarilerindeki paralel işlem kapasitesini, bellek sistemlerinin performans üzerindeki belirleyici rolünü ve paralel algoritmaların tasarım prensiplerini akademik bir titizlikle ele almaktadır. Notlar, Grama ve ark. (2003) tarafından sunulan kuramsal ve pratik temeller üzerine inşa edilmiştir.

## 1. Giriş: Modern İşlemci Gelişimi ve Paralelliğin Gerekliliği

Mikroişlemci teknolojisi, Moore Yasası olarak bilinen ve devre karmaşıklığının yaklaşık her 18 ayda bir ikiye katlanacağını öngören ampirik gözlem doğrultusunda devasa bir gelişim göstermiştir. Tarihsel süreçte, 1988'de 40 MHz (MIPS R3000) olan saat hızları, 2002'de 2.0 GHz (Pentium 4) seviyelerine ulaşarak yaklaşık 50 kat artmıştır. Ancak bu gelişim süreci, sistem performansını etkileyen kritik darboğazları da beraberinde getirmiştir.

- **Hesaplama Gücü (FLOPS):** Transistör sayısındaki artış, işlemcilerin saniyede yapabildiği kayan nokta operasyonu sayısını (FLOPS) dramatik şekilde artırmıştır.
- **İşlemci-Bellek Uçurumu:** İşlemci saat hızları yıllık %40 oranında iyileşirken, DRAM erişim süreleri (latency) yalnızca %10 oranında iyileşebilmiştir. Bu asimetri, işlemcinin veri beklerken atıl kalmasına neden olan "bellek duvarı" (memory wall) sorununu doğurmuştur.
- **Veri İletişimi:** Modern sistemlerde performans, sadece verinin ne kadar hızlı işlendiğiyle değil, bellek hiyerarşisi içindeki iletişim maliyetleriyle sınırlıdır.

## 2. Mikroişlemci Mimarilerinde Örtük Paralellik (Implicit Parallelism)

Örtük paralellik, programcının müdahalesi olmadan donanım ve derleyici seviyesinde sağlanan paralelliktir.

### 2.1. Boru Hattı (Pipelining) ve Süper-skaler Yürütme

Pipelining, komut işlemeyi bir "montaj hattına" dönüştürür. Fetch (getirme), decode (çözme), execute (yürütme) ve write-back (geri yazma) gibi aşamalar üst üste bindirilerek, her bir komutun toplam süresi azalmasa da, birim zamanda tamamlanan komut sayısı artırılır.

Süper-skaler işlemciler (Şekil 2.1), aynı anda birden fazla komutu işleme alabilen birden fazla boru hattına sahiptir. Ancak bu mimaride üç temel bağımlılık türü performansı sınırlar:

1. **Gerçek Veri Bağımlılığı (True Data Dependency):** Bir komutun sonucu, bir sonraki komutun girdisi olduğunda ortaya çıkar.
2. **Kaynak Bağımlılığı (Resource Dependency):** İki komutun aynı anda aynı donanım birimine (örn. tek bir kayan nokta ünitesi) erişmek istemesidir.
3. **Yordamsal Bağımlılık (Procedural Dependency):** Dallanma (branch) komutları nedeniyle bir sonraki komutun adresinin belirsiz olmasıdır. Bu durum, yanlış dallanma tahmini yapıldığında boru hattının boşaltılmasına (pipeline flush) ve ciddi performans kaybına yol açar.

### 2.2. VLIW (Çok Uzun Komut Kelimesi) İşlemciler

VLIW mimarisi, süper-skaler mimarinin karmaşık bağımlılık analiz donanımını ortadan kaldırarak bu yükü derleyiciye aktarır. Bağımsız komutlar derleme zamanında tek bir uzun kelime içinde paketlenir.

| Özellik | Süper-skaler Mimari | VLIW Mimari (IA64 vb.) |
| :--- | :--- | :--- |
| **Zamanlama Kararı** | Çalışma zamanı (Donanım) | Derleme zamanı (Yazılım) |
| **Bağımlılık Analizi** | Karmaşık donanım mantığı | Derleyici optimizasyonu |
| **Donanım Karmaşıklığı** | Yüksek | Düşük |
| **Verimlilik Kaybı** | Dinamik stall'lar | Yatay ve Dikey Atık (Waste) |

VLIW sistemlerde, bir çevrimde bazı işlem birimlerinin boş kalması yatay atık (horizontal waste), hiçbir komut paketinin gönderilememesi ise dikey atık (vertical waste) olarak tanımlanır (Şekil 2.1c).

## 3. Bellek Sistemi Performansı ve Sınırlamalar

Geleneksel von Neumann mimarisindeki darboğazı aşmak için modern sistemler bellek hiyerarşisini kullanır.

### 3.1. Gecikme (Latency) vs. Bant Genişliği (Bandwidth)

Grama ve ark. (2003), bu ayrımı bir itfaiye hortumu analojisiyle açıklar:

- **Gecikme:** Musluk açıldıktan sonra suyun hortumun ucundan çıkması için geçen süredir (nanosaniye).
- **Bant Genişliği:** Hortumun birim zamanda aktarabildiği su miktarıdır (GB/s).

### 3.2. Önbellek (Cache) ve Yerellik İlkesi

Önbellek performansı, Yerellik İlkesi (Locality of Reference) üzerine kuruludur:

- **Zamansal Yerellik (Temporal):** Bir veri öğesine erişildiğinde, yakın gelecekte tekrar erişilme olasılığının yüksek olması.
- **Mekansal Yerellik (Spatial):** Erişilen bir verinin bitişiğindeki verilere erişilme olasılığının yüksek olması.

> **Örnek 2.3 (Matris Çarpımı Analizi):** 1 GHz hızında bir işlemcide, 100 ns DRAM gecikmesi olan bir sistemde $32 \times 32$ matris çarpımı yapıldığında:
> 
> - 2K kelimenin (matrisler A ve B) önbelleğe getirilmesi yaklaşık $200~\mu s$ sürer.
> - 64K operasyonun (4 komut/çevrim kapasitesiyle) icrası $16~\mu s$ sürer.
> - Toplam süre: $216~\mu s$. Performans: 303 MFLOPS.
> - Önbellek olmasaydı (No-cache scenario), her erişimde 100 çevrim bekleneceği için performans yaklaşık 10 MFLOPS seviyesine düşecekti.

## 4. Veri Odaklı Tasarım: Veri Erişim Desenleri ve Optimizasyon

Yazılımın bellek dizilimi, donanım verimliliğini doğrudan belirler.

### 4.1. Adımlı Erişim (Strided Access) Problemi

Örnek 2.5'te görüldüğü gibi, matris verisinin bellek dizilimine ters yönde erişilmesi mekansal yerelliği yok eder.

```c
// Örnek 2.5: Kötü Mekansal Yerellik (Sütun Öncelikli Erişim / Column-Major Access)
for (int i = 0; i < 1000; i++) {
    for (int j = 0; j < 1000; j++) {
        column_sum[i] += b[j][i]; // Her erişimde yeni cache line yüklenir
    }
}
```

### 4.2. Döngü Döşeme (Tiling)

Tiling tekniği, büyük veri setlerini önbelleğe sığacak küçük bloklara bölerek bellek bant genişliği gereksinimini azaltır (Örnek 2.6).

```c
// Örnek 2.6: Tiling (Bloklama) Mantığı
for (int ii = 0; ii < n; ii += B) {
    for (int jj = 0; jj < n; jj += B) {
        for (int i = ii; i < min(ii + B, n); i++) {
            for (int j = jj; j < min(jj + B, n); j++) {
                // Blok bazlı işlem yaparak veriyi cache'de tutuyoruz
            }
        }
    }
}
```

### 4.3. AoS vs. SoA

- **AoS (Array of Structures):** Verinin nesne tabanlı (örn. `[x,y,z, x,y,z]`) dizilimi.
- **SoA (Structure of Arrays):** Verinin öznitelik tabanlı (örn. `[x,x,x], [y,y,y]`) dizilimi.
- **Teknik Not:** SoA, SIMD (Vektörel) birimlerin aynı özniteliği birden fazla kayıt için bitişik bellekten tek seferde yüklemesine (contiguous loading) olanak tanıdığı için tercih edilir.

## 5. Bellek Gecikmesini Gizleme Teknikleri

### 5.1. Çoklu İş Parçacığı (Multithreading)

İşlemci, bir iş parçacığı bellekten veri beklerken (stall), diğer bir iş parçacığına geçiş yaparak yürütme birimlerini dolu tutar (Örnek 2.7).

### 5.2. Önceden Getirme (Prefetching)

Verinin ihtiyaç duyulmadan önce donanım veya yazılım tarafından önbelleğe yüklenmesidir (Örnek 2.8). Ancak prefetching, yanlış tahmin edilirse veya aşırı yapılırsa bellek bant genişliğini gereksiz yere işgal edebilir.

> **Vurgu Kutusu: Bant Genişliği ve Trade-off (Örnek 2.9)**
> Multithreading, gecikmeyi gizlese de toplam bant genişliği ihtiyacını artırır. Tek bir thread %90 hit oranıyla 400 MB/s bant genişliği talep ederken; 32 thread kullanıldığında her birinin cache payı azalacağından (örn. %25 hit oranı), toplam ihtiyaç 3 GB/s seviyesine çıkabilir.

## 6. Performans Limitleri ve Analiz Modelleri

**STREAM Benchmark Analizi (Örnek 2.4):** Bir vektör nokta çarpımı (dot-product) işleminde, 100ns gecikmeli bir sistemde:

- Blok boyutu 1 kelime ise: $100 \text{ ns}$'de 1 kelime çekilir $\rightarrow$ 10 MFLOPS.
- Blok boyutu 4 kelime (cache line) ise: $100 \text{ ns}$'de çekilen 4 kelime sonraki 4 operasyonun verisini sağlar. Bu durum, mekansal yerellik sayesinde gecikmenin 4 operasyona amortize edilmesini sağlar ve performansı 40 MFLOPS seviyesine çıkarır.

## 7. Paralel Platformlar ve Kontrol Yapıları

- **SIMD (Single Instruction, Multiple Data):** Tek bir komutun birden fazla veri üzerinde (örn. vektör işlemcileri) icrasıdır.
  > **Örnek 2.11:** SIMD mimarisinde `if-else` blokları performansı düşürür; çünkü `if` bloğunu çalıştıran işlemciler varken `else` kısmındakiler atıl beklemek zorundadır (masking).
- **MIMD (Multiple Instruction, Multiple Data):** Her işlemcinin bağımsız program ve veri akışına sahip olmasıdır.
- **Paylaşılan Adres Uzayı:**
  - **UMA:** Tüm işlemciler tüm belleğe aynı sürede erişir.
  - **NUMA (Şekil 2.5):** İşlemcinin yerel belleğine erişimi, diğer işlemcilerin yerel (uzak) belleklerine erişiminden daha hızlıdır.

## 8. Algoritma Tasarım Prensipleri ve Görev Bağımlılıkları

Paralel algoritma tasarımında problem görevlere (tasks) ayrıştırılır ve bu görevler arasındaki bağımlılıklar bir Görev Bağımlılık Grafiği (DAG) ile gösterilir.

### 8.1. Temel Formülasyonlar

Grama ve ark. (2003) uyarınca, paralellik derecesini belirleyen metrikler:

- **Toplam İş (W):** Tüm görevlerin toplam süresi.
- **Kritik Yol Uzunluğu (L):** Grafikteki en uzun bağımlılık yolu.
- **Ortalama Paralellik Derecesi (Avg Concurrency):**
  $$ \text{Avg Concurrency} = \frac{W}{L} $$

### 8.2. Veritabanı Sorgu İşleme Analizi (Şekil 3.2 ve 3.3)

İki farklı ayrıştırma stratejisinin karşılaştırmalı analizi:

**1. Strateji (a):**
- Toplam İş (W): 63 birim.
- Kritik Yol (L): 27 birim.
- Ortalama Paralellik: $63 / 27 \approx \mathbf{2.33}$

**2. Strateji (b):**
- Toplam İş (W): 64 birim.
- Kritik Yol (L): 34 birim.
- Ortalama Paralellik: $64 / 34 \approx \mathbf{1.88}$

> **Akademik Sonuç:** Strateji (a), daha kısa bir kritik yola ve daha yüksek bir ortalama paralellik derecesine sahip olduğu için daha verimli bir paralel yapı sunar. Algoritma tasarımında temel hedef, kritik yolu minimize ederek işlemci kaynaklarını maksimize etmektir.
