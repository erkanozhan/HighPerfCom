# Weka ile Dağıtık İşlem: WekaServer ile Ana–Yardımcı Sunucu Kurulumu

Bu rehber, Weka'nın `wekaServer` paketini kullanarak analiz görevlerinin ağdaki birden fazla makinede nasıl yürütüleceğini adım adım açıklar. Mimari, bir **ana sunucu** (master) ve ona bağlı bir veya daha fazla **yardımcı sunucudan** (slave) oluşur.

## 0. WekaServer Ne Yapar, Ne Yapmaz?

WekaServer (Mark Hall), HTTP üzerinden görev kabul eden küçük bir sunucudur. Bir görev, örneğin Explorer'da başlatılan bir sınıflandırıcı değerlendirmesi ya da bir Knowledge Flow akışı, **bütün olarak** bir sunucuda çalışır.

Ana sunucu, kendisine gelen her görev için önce kendi yükünü hesaplar:

$$\text{yük} = \frac{(\text{çalışan görev} + \text{kuyruktaki görev}) \times \text{load-adjust}}{\text{slots}}$$

```math
\underbrace{\text{yük}}_{\substack{\text{sunucunun}\\\text{doluluk oranı}}} = \frac{\overbrace{(\text{çalışan görev} + \text{kuyruktaki görev})}^{\substack{\text{sunucudaki}\\\text{toplam görev}}} \times \overbrace{\text{load-adjust}}^{\substack{\text{yük}\\\text{çarpanı}}}}{\underbrace{\text{slots}}_{\substack{\text{aynı anda yürütülebilen}\\\text{görev sayısı}}}}
```

**Ne anlatıyor?** Sunucuda çalışan ve sırada bekleyen görevlerin toplamı, sunucunun aynı anda yürütebileceği görev sayısına bölünerek doluluk oranı bulunur; load-adjust çarpanı bu oranı yapay olarak büyütür ya da küçültür. Oranın 1'e ulaşması, sunucunun kapasitesinin dolduğunu gösterir.

Kendi yükü 1'in altındaysa görevi kendisi yürütür; değilse görevi yükü en düşük yardımcı sunucuya gönderir.

Bu nedenle WekaServer **veriyi bölmez**. Çok sayıda bağımsız görevi (farklı algoritmalar, farklı parametreler, farklı veri kümeleri) makinelere dağıtarak toplam süreyi kısaltır; tek bir büyük veri kümesinin parçalara ayrılıp paralel işlenmesi gerekiyorsa Spark tabanlı dağıtık Weka kullanılmalıdır (bkz. [weka_dagitik_spark_giris.md](weka_dagitik_spark_giris.md)).

## 1. Önkoşullar

Tüm ana ve yardımcı makinelerde aşağıdaki gereksinimler karşılanmalıdır.

### a. Weka ve Java

- Weka 3.8.x kurulu olmalıdır. Windows kurulumu kendi Java çalışma ortamıyla gelir (Weka 3.8.6'da `C:\Program Files\Weka-3-8-6\jre` dizinindeki Java 17).
- Komut satırından çalıştırırken kullanılan `java` komutunun sürümü şu komutla görülebilir:

  ```bash
  java -version
  ```

- **Windows'ta birden fazla Java sürümü varsa:** `java` komutu, `Path` değişkeninde ilk sırada bulunan Java'yı çalıştırır. İstenen Java'nın `bin` dizinini `Path` listesinde yukarı taşıyın ya da Weka ile gelen Java'yı tam yoluyla çağırın:

  ```bat
  "C:\Program Files\Weka-3-8-6\jre\zulu17.32.13-ca-fx-jre17.0.2-win_x64\bin\java.exe" -version
  ```

  (Dizin adı Weka sürümüne göre değişir; kendi kurulumunuzdaki `jre` klasörüne bakınız.)
- **Linux'ta (Ubuntu/Debian):**

  ```bash
  sudo apt update
  sudo apt install openjdk-17-jre-headless
  # Birden fazla Java varsa varsayılanı seçmek için:
  sudo update-alternatives --config java
  ```

### b. `wekaServer` Paketi

WekaServer, çekirdek `weka.jar` içinde **yer almaz**; ayrı bir pakettir ve her makineye kurulmalıdır.

- **Grafik arayüzle:** Weka GUI Chooser → **Tools → Package Manager** → listeden `wekaServer` → **Install**. Kurulumdan sonra Weka'yı yeniden başlatın.
- **Komut satırıyla:**

  ```bash
  java -cp weka.jar weka.core.WekaPackageManager -install-package wekaServer
  ```

Paket, kullanıcının ev dizinindeki `wekafiles/packages/wekaServer/` klasörüne kurulur. Aşağıdaki komutlarda kullanılan `weka.Run` sınıfı kurulu paketleri kendiliğinden yükler.

### c. Ağ Yapılandırması ve Güvenlik Duvarı

Ana ve yardımcı makinelerin belirtilen portlar üzerinden birbirine erişebilmesi gerekir.

**Windows Defender Güvenlik Duvarı:**

1. Sunucu ilk kez başlatıldığında Windows bir uyarı penceresi gösterebilir; Java'nın ağ erişimine **İzin ver** seçeneğiyle onay verin.
2. Uyarı görünmezse: Denetim Masası → Sistem ve Güvenlik → Windows Defender Güvenlik Duvarı → Gelişmiş ayarlar → Gelen Kuralları → Yeni Kural → Bağlantı Noktası → TCP 8085.

**Linux (`ufw`):**

```bash
sudo ufw status                 # durumu görüntüle
sudo ufw allow 8085/tcp         # portu aç
sudo ufw status                 # değişikliği doğrula
```

### d. IP Adreslerinin Belirlenmesi

- **Windows:** Komut isteminde `ipconfig` komutunu çalıştırıp "IPv4 Address" satırına bakın.
- **Linux:** `ip addr` komutunun çıktısında `inet` ile başlayan satıra bakın.

Bu rehberdeki örneklerde ana sunucunun adresi **192.168.1.10**, yardımcı sunucularınki **192.168.1.21** ve **192.168.1.22** olarak varsayılmıştır. Kendi adreslerinizi kullanınız.

---

## 2. Ana Sunucunun (Master) Başlatılması

**Genel komut yapısı:**

```bash
java -cp <WEKA_JAR_YOLU> -Xmx<EN_FAZLA_BELLEK> -Djava.awt.headless=true weka.Run WekaServer \
  -host <ANA_SUNUCU_IP> -port <PORT> -slots <SLOT_SAYISI> -load-adjust <KATSAYI> -staleTime <MİLİSANİYE>
```

**Örnek (Windows, 8 GB bellek):**

```bat
java -cp "C:\Program Files\Weka-3-8-6\weka.jar" -Xmx8G -Djava.awt.headless=true weka.Run WekaServer -host 192.168.1.10 -port 8085 -slots 1 -load-adjust 4 -staleTime 3600000
```

**Parametreler** (anlamları paketin kaynak kodundan, `weka.server.WekaServer`, doğrulanmıştır):

| Parametre | Anlamı | Varsayılan |
| :--- | :--- | :--- |
| `-cp` / `-classpath` | `weka.jar` dosyasının yolu | — |
| `-Xmx8G` | Java sanal makinesine ayrılan en fazla yığın bellek (Java seçeneği) | Java'ya bağlı |
| `-Djava.awt.headless=true` | Grafik arayüzü olmayan ortamda çalışmayı sağlar (Java seçeneği) | — |
| `-host` | Sunucunun dinleyeceği adres. Yalnızca kendi makinenizde deneme yapacaksanız `localhost`; ağda çalışacaksanız makinenin IP adresi | makine adı |
| `-port` | Dinlenecek TCP portu | 8085 |
| `-slots` | Bu sunucunun aynı anda yürütebileceği görev sayısı | 2 |
| `-load-adjust` | Yük hesabındaki çarpan (bkz. Bölüm 0). 1'den büyük bir değer, ana sunucunun kendisini daha yüklü saymasına ve görevleri daha erken yardımcı sunuculara devretmesine yol açar | 1.0 |
| `-staleTime` | Tamamlanmış ya da başarısız olmuş görevlerin sunucudan **temizlenme** süresi, **milisaniye** cinsinden. `-1` temizlemeyi kapatır | 3600000 (1 saat) |
| `-daemon` | Sunucuyu arka plan hizmeti olarak çalıştırır | kapalı |

`-staleTime` değeri sunucunun çalışma süresini sınırlamaz ve **çalışmakta olan** görevlere dokunmaz; yalnızca bitmiş görevlerin sonuçlarının bellekte ne kadar tutulacağını belirler. Çok kısa bir değer verilirse istemci, uzun süren bir görevin sonucunu almadan önce sonuç silinmiş olabilir; sonuçları geç toplayacaksanız değeri büyütün ya da `-1` kullanın.

Sunucu çalışırken bir tarayıcıda `http://192.168.1.10:8085/` adresi açılarak çalışan ve kuyruktaki görevler ile kayıtlı yardımcı sunucular görülebilir.

---

## 3. Yardımcı Sunucuların (Slave) Ana Sunucuya Bağlanması

Her yardımcı sunucu kendi makinesinde ayrı bir komut olarak başlatılır ve `-master` seçeneğiyle ana sunucuya kaydolur.

**Genel komut yapısı:**

```bash
java -cp <WEKA_JAR_YOLU> -Xmx<EN_FAZLA_BELLEK> -Djava.awt.headless=true weka.Run WekaServer \
  -host <BU_MAKİNENİN_IP_ADRESİ> -port <PORT> -master <ANA_SUNUCU_IP>:<ANA_SUNUCU_PORTU> -slots <SLOT_SAYISI>
```

**Örnek (Windows, 6 GB bellek, 4 görev yuvası; 192.168.1.21 adresli makinede):**

```bat
java -cp "C:\Program Files\Weka-3-8-6\weka.jar" -Xmx6G -Djava.awt.headless=true weka.Run WekaServer -host 192.168.1.21 -port 8085 -master 192.168.1.10:8085 -slots 4
```

İkinci yardımcı sunucu (192.168.1.22) için aynı komutta yalnızca `-host` değeri değiştirilir.

- `-host`: Komutun çalıştırıldığı **yardımcı** makinenin kendi IP adresi.
- `-master`: Ana sunucunun IP adresi ve portu.
- `-slots 4`: Bu makinenin aynı anda dört görev yürütebileceğini belirtir. Değer, makinenin çekirdek sayısına ve belleğine göre seçilmelidir; her görev kendi veri kopyasını belleğe yükler.

Bağlantı kurulduğunda ana sunucunun web sayfasında yardımcı sunucular "Slaves" başlığı altında listelenir.

---

## 4. Görevlerin Sunucuya Gönderilmesi

`wekaServer` paketi Weka arayüzüne iki giriş noktası ekler:

1. **Explorer → Classify sekmesi:** Paket kurulduktan sonra **Start** düğmesinin yanında **Run on server** düğmesi belirir. Bir sınıflandırıcı ve test seçeneği belirlendikten sonra bu düğmeye basılır, açılan pencerede sunucunun adresi ve portu girilir. Değerlendirme görevi sunucuya gönderilir; sonuç, görev bitince Explorer'daki sonuç listesine eklenir.
2. **Knowledge Flow → "Remote host scheduler" görünümü (perspective):** Kaydedilmiş bir akış sunucuya gönderilir; isteğe bağlı olarak belirli zamanlarda yinelenecek biçimde zamanlanabilir.

Ana sunucuya gönderilen görevler Bölüm 0'daki kurala göre ana sunucuda ya da yükü en düşük yardımcı sunucuda yürütülür. Örneğin on farklı sınıflandırıcı yapılandırması ayrı ayrı gönderildiğinde, görevler yuva sayısına göre makinelere yayılır ve toplam süre kısalır.

---

## 5. Sık Karşılaşılan Sorunlar

- **`WekaServer` sınıfı bulunamıyor:** `wekaServer` paketi o makinede kurulu değildir (Bölüm 1.b).
- **Yardımcı sunucu ana sunucuya kaydolmuyor:** Güvenlik duvarı ayarlarını, `-master` adresini ve iki makinenin birbirine `ping` ile erişip erişemediğini kontrol edin. `-host` değeri olarak `localhost` verilen bir sunucuya başka makineler bağlanamaz.
- **Bellek yetersizliği (`OutOfMemoryError`):** `-Xmx` değerini artırın ya da `-slots` değerini azaltın.
