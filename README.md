# HighPerfCom — Yüksek Başarımlı Hesaplama

Bu depo, Tekirdağ Namık Kemal Üniversitesi Çorlu Mühendislik Fakültesi Bilgisayar Mühendisliği Bölümü lisansüstü **Yüksek Başarımlı Hesaplama** (High-Performance Computing, HPC) dersinin notlarını ve uygulamalarını içerir.

## Önerilen Okuma Sırası

| Sıra | Dosya | İçerik |
| :---: | :--- | :--- |
| 1 | [Ders_Notu.md](Ders_Notu.md) | Paralel mimariler, başarım ölçütleri (Amdahl, Gustafson), bellek hiyerarşisi, veri odaklı tasarım, Roofline modeli, görev bağımlılıkları ve OpenMP |
| 2 | [Uygulamalar/uygulamalar.md](Uygulamalar/uygulamalar.md) | OpenMP kurulumu, derleme ve ölçülmüş çıktılarıyla örnek programlar |
| 3 | [weka_server_setup.md](weka_server_setup.md) | WekaServer paketiyle görevlerin ağdaki makinelere dağıtılması |
| 4 | [weka_dagitik_spark_giris.md](weka_dagitik_spark_giris.md) | Dağıtık Weka ve Apache Spark: MapReduce, temel kavramlar, Knowledge Flow şablonları |
| 5 | [weka_spark_kullanimi.md](weka_spark_kullanimi.md) | Spark kavramları (RDD, tembel değerlendirme, DAG) ve Spark işlerinin komut satırından çalıştırılması |
| 6 | [weka_knowledgeflow_spark.md](weka_knowledgeflow_spark.md) | Spark işlerinden Knowledge Flow'da akış kurulması, kaydedilmesi ve sunucuda çalıştırılması |

## Dizin Yapısı

```text
Ders_Notu.md                 Ana ders notu
Uygulamalar/
  uygulamalar.md             Uygulama kılavuzu
  OpenMP/
    merhaba.c                İlk paralel bölge
    seri_toplama.c           Seri toplam (karşılaştırma için)
    parallel_toplama.c       reduction ile paralel toplam
    pi_hesaplama.c           Sayısal integralle pi
    vektor_toplama.c         Büyük vektörlerde veri paralelliği
    yaris.c                  Yarış durumu: korumasız, atomic, critical, reduction
    hizlanma.c               Hızlanma, verimlilik ve Karp-Flatt ölçümü
    zamanlama.c              static, dynamic, guided zamanlama deneyi
    paylasim.c               private/firstprivate/lastprivate ve yanlış paylaşım
weka_*.md                    Dağıtık Weka notları
images/                      Şekiller (SVG)
```

## Gereksinimler

- **C derleyicisi:** OpenMP destekli GCC. Windows'ta MSYS2 ile kurulumu `Uygulamalar/uygulamalar.md` dosyasında anlatılmıştır; Linux'ta dağıtımın `gcc` paketi yeterlidir. Derleme: `gcc -O2 -fopenmp -o program program.c`.
- **Weka 3.8.x:** <https://waikato.github.io/weka-wiki/downloading_weka/>. Windows kurulumu kendi Java çalışma ortamıyla gelir.
- **Weka paketleri** (Tools → Package Manager):
  - `wekaServer` — WekaServer ile dağıtık görev yürütme için.
  - `distributedWekaSpark` (1.0.x; `distributedWekaBase` bağımlılığı kendiliğinden kurulur) — Spark notları için.
- Notlardaki ölçümler Intel Core i7-4790K (4 çekirdek / 8 iş parçacığı), Windows 11 ve MSYS2 GCC 14.2 ile alınmıştır; kendi makinenizde farklı değerler elde etmeniz olağandır.
