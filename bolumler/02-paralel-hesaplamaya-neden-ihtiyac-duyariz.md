# Bölüm 2 — Paralel Hesaplamaya Neden İhtiyaç Duyarız?

## 2.1 Moore Yasası ve Tek Çekirdek Başarımının Tarihi

**Moore Yasası** (Moore's Law), Intel'in kurucularından Gordon Moore'un 1965'te yaptığı ampirik bir gözlemdir: Bir tümleşik devreye sığdırılabilen transistör sayısı düzenli aralıklarla ikiye katlanır. Moore bu süreyi önce bir yıl olarak vermiş, 1975'te yaklaşık iki yıla düzeltmiştir. Sıkça anılan "18 ay" değeri ise başarım artışı için yapılan bir yorumdur ve Grama ve ark. (2003) de bu değeri kullanır. Transistör sayısındaki artış, uzun yıllar boyunca hem daha yüksek saat frekansı hem de daha karmaşık işlemci tasarımları olarak kullanıcıya yansımıştır.

Hennessy ve Patterson'ın (2019, Şekil 1.1) derlediği verilere göre tek işlemcili sistemlerin başarımı yıllık ortalama şu oranlarda artmıştır:

| Dönem | Yıllık başarım artışı |
| :--- | :---: |
| 1978–1986 | yaklaşık %25 |
| 1986–2003 | yaklaşık %52 |
| 2003–2011 | yaklaşık %23 |
| 2011–2015 | yaklaşık %12 |
| 2015–2018 | yaklaşık %3,5 |

Saat frekansı da 1986–2003 arasında yılda yaklaşık %40 artmış, 2003 sonrasında ise neredeyse durmuştur. Bugün masaüstü işlemcilerin frekansı birkaç GHz düzeyinde kalmaktadır.

## 2.2 Güç Duvarı ve Dennard Ölçeklemesinin Sona Ermesi

Bir CMOS devrenin dinamik güç tüketimi yaklaşık olarak şu bağıntıyla verilir:

$$P_{\text{dinamik}} \approx \alpha \cdot C \cdot V^{2} \cdot f$$

```math
\underbrace{P_{\text{dinamik}}}_{\substack{\text{dinamik}\\\text{güç}}} \approx \overbrace{\alpha}^{\substack{\text{anahtarlama}\\\text{etkinliği}}} \cdot \underbrace{C}_{\substack{\text{anahtarlanan}\\\text{sığa}}} \cdot \overbrace{V^{2}}^{\substack{\text{besleme gerilimi}\\\text{(karesi)}}} \cdot \underbrace{f}_{\substack{\text{saat}\\\text{frekansı}}}
```

**Ne anlatıyor?** Devrenin her saat çevriminde anahtarlanan sığayı doldurup boşaltmak için harcadığı güç verilir. Güç frekansla doğrusal, gerilimle ise karesel olarak arttığından, gerilimin düşürülemediği durumda frekans artışı güç tüketimini hızla büyütür.

Burada $\alpha$ anahtarlama etkinliği, $C$ anahtarlanan sığa, $V$ besleme gerilimi ve $f$ saat frekansıdır. **Dennard ölçeklemesi** (Dennard scaling; Robert Dennard, 1974), transistörler küçüldükçe gerilim ve sığanın da aynı oranda küçüleceğini, böylece birim alandaki güç yoğunluğunun sabit kalacağını öngörüyordu. Yaklaşık 2005'ten itibaren sızıntı akımları ve eşik gerilimi nedeniyle gerilim artık düşürülemez hâle geldi. Bu durumda frekansı artırmak için gerilimin de artırılması gerektiğinden güç, frekansın yaklaşık küpüyle ($V \propto f$ varsayımıyla $P \propto f^3$) büyür. Ortaya çıkan ısı soğutma sınırlarına takılır; bu sınıra **güç duvarı** (power wall) denir.

Bu nedenle üreticiler tek ve çok hızlı bir çekirdek yerine aynı yonga üzerinde birden fazla çekirdeğe (multicore) yöneldi. Bir mutfakta tek bir aşçıyı ısıdan bayılacak kadar hızlı çalıştırmak yerine işi normal hızda çalışan dört aşçıya dağıtmak buna benzer.

Sonuç: Yeni donanım almak eski seri yazılımları artık kendiliğinden hızlandırmaz; programcının hesabı paralel alt parçalara bölmesi gerekir.

---

[← Bölüm 1 — Giriş: Bilgisayarlar ve Sayıların Temsili](01-giris-bilgisayarlar-ve-sayilarin-temsili.md) · [İçindekiler](../Ders_Notu.md) · [Bölüm 3 — Paralel Mimariler ve Flynn Taksonomisi →](03-paralel-mimariler-ve-flynn-taksonomisi.md)
