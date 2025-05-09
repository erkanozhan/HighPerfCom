
# Weka ile Dağıtık İşlem: Master-Slave Kurulumu ve Kullanımı

Bu rehber, Weka'nın dağıtık işlem (WekaServer) özelliğini kullanarak büyük veri kümeleri üzerinde analiz görevlerini birden fazla makineye (slave) dağıtarak nasıl çalıştırılacağını adım adım açıklar. Bu mimari, bir "master" sunucu ve ona bağlı bir veya daha fazla "slave" sunucudan oluşur.

## 1. Önkoşullar

Tüm master ve slave makinelerde aşağıdaki gereksinimlerin karşılandığından emin olun:

### a. Java Runtime Environment (JRE)

Weka, Java tabanlıdır. Genellikle Java 8 (1.8) sürümü önerilir.

**Ubuntu/Debian için Java 1.8 JRE Kurulumu:**

```bash
sudo apt update
sudo apt install openjdk-8-jre-headless
```

**Java Sürümünü Doğrulama:**

```bash
java -version
```

Çıktı şuna benzer olmalıdır:

```bash
java version "1.8.0_XYZ"
Java(TM) SE Runtime Environment (build 1.8.0_XYZ-bAA)
Java HotSpot(TM) 64-Bit Server VM (build DD.XYZ-bAA, mixed mode)
```

**Windows için Java:**

Oracle veya AdoptOpenJDK (Temurin) gibi resmi kaynaklardan Java 8 JRE veya JDK'yı indirip kurun. Windows'ta birden fazla Java sürümünü yönetmek için şu adımları izleyin:

- Java 1.8'i yükleyin.
- Daha yüksek Java sürümlerine ihtiyaç duyan yazılımlarınız varsa, bu sürümleri kaldırmayın.
- Java 1.8'i öncelikli yapmak için:
  1. Windows ortam değişkenlerini açın.
  JAVA_HOME değişkeni açarak değerini "C:\Program Files\Java\jre-1.8" yapın. Ortamda örneğin Java 21 de yüklüyse bu durumda System Variables başlığı altında (Ortam değişkenleri penceresinin altındaki bölümde) bulunan Path değişkeninin içeriğini açarak Java 1.8'in yolunu diğer sürümlerin üstüne taşıyın. Alternatif olarak, birden fazla Java sürümünü yönetmek için `jenv` gibi bir Java sürüm yöneticisi kullanabilirsiniz. Bu araç, farklı projeler için kolayca Java sürümleri arasında geçiş yapmanıza olanak tanır.

  Değişikliklerden sonra aktif Java sürümünü doğrulamak için aşağıdaki komutu çalıştırın:
  ```bash
  java -version
  ```
  Bu komut, sistemde etkin olan Java sürümünü gösterecektir.
   - `JAVA_HOME` değişkenini "C:\Program Files\Java\jre-1.8" olarak ayarlayın.

2. Eğer sistemde birden fazla Java sürümü yüklüyse:
   - `Path` değişkenini düzenleyin ve "C:\Program Files\Common Files\Oracle\Java\javapath" yolunun sonuna bir rakam ekleyerek (örneğin, "2") Java 21'in devreye girmesini engelleyin.

3. Java 21'e tekrar ihtiyaç duyarsanız:
   - "2" rakamını kaldırın veya `JAVA_HOME` değerini istediğiniz Java sürümüne ayarlayın.

**Örnek:**
- `JAVA_HOME`: "C:\Program Files\Java\jre-1.8"
- `Path`: "C:\Program Files\Common Files\Oracle\Java\javapath2"
  2. PATH değişkeninde Java 1.8'in yolunu bulun ve diğer sürümlerin üstüne taşıyın.

### b. Weka JAR Dosyası

Weka'nın ana `.jar` dosyasına (örneğin, `weka.jar` veya `weka-3-8-6.jar`) ihtiyacınız olacak. Bu dosya, hem master hem de tüm slave makinelerde bulunmalı ve komutlar çalıştırılırken erişilebilir olmalıdır.

**Bulunması Gereken Klasör (Örnek):**

Windows'ta Weka kurulumu genellikle:

```
C:\Program Files\Weka-3-8-6\
```

Altında olur ve `weka.jar` bu dizinde bulunur.

### c. Ağ Yapılandırması ve Güvenlik Duvarı (Firewall)

Master ve slave makinelerin birbirleriyle belirtilen portlar üzerinden iletişim kurabilmesi gerekir.

**Windows Güvenlik Duvarı:**

1. Windows Defender Güvenlik Duvarı ayarlarında, Java'nın (`javaw.exe` veya `java.exe`) ağ erişimine izin verildiğinden emin olun.  
2. Ayarlar:  
   `Denetim Masası` → `Sistem ve Güvenlik` → `Windows Defender Güvenlik Duvarı` → `Bildirim ayarlarını değiştir`.  
3. `Windows Defender Güvenlik Duvarı yeni bir uygulamayı engellediğinde bana bildir` seçeneğini işaretleyin.  
4. Gerekirse, WekaServer'ın kullanacağı port (örneğin 8080 veya 8085) için manuel olarak gelen/giden kurallar ekleyin.

**Linux Güvenlik Duvarı (örneğin `ufw`):**

```bash
sudo ufw allow 8085/tcp
sudo ufw reload
```

---

## 2. Master Server'ı Başlatma

Master server, Weka görevlerini kabul eder ve uygun slave'lere dağıtır. Master makinenin IP adresini öğrenmek için:

- **Windows**: Komut istemcisini (cmd) açın ve `ipconfig` komutunu çalıştırın. Çıktıda "IPv4 Address" satırını bulun.
- **Linux**: Terminalde `ifconfig` veya `ip addr` komutlarından birini çalıştırın. Çıktıda "inet" ile başlayan satırda IP adresini görebilirsiniz.

**Genel Komut Yapısı:**

```bash
java -cp <WEKA_JAR_YOLU> -Xmx<MAKS_BELLEK> -Djava.awt.headless=true weka.Run WekaServer \
  -host <MASTER_IP_ADRESİ_VEYA_LOCALHOST> -port <PORT_NUMARASI> \
  -load-adjust <YÜK_AYARI> -slots <SLOT_SAYISI> -staleTime -1
```

**Parametre Açıklamaları:**

- `-cp <WEKA_JAR_YOLU>`: Weka'nın .jar dosyasının yolu.
- `-Xmx<MAKS_BELLEK>`: Master server'a ayrılacak maksimum Java heap belleği.
- `-Djava.awt.headless=true`: GUI olmayan ortamlarda çalışmayı sağlar.
- `-host`: Master sunucunun IP adresi veya `localhost`.
- `-port`: Dinlenecek port.
- `-load-adjust`: Genelde `4` olarak ayarlanır.
- `-slots`: Master'ın kaç görevi eş zamanlı işleyebileceği.
- `-staleTime -1`: Görev zaman aşımı yoktur.

**Örnek Komut (localhost, 5GB RAM):**

```bash
java -cp weka.jar -Xmx5000m -Djava.awt.headless=true weka.Run WekaServer -host localhost -port 8085 -load-adjust 4 -slots 1 -staleTime -1
```
Bu komutu başlattıktan sonra, **Windows** ortamlarında güvenlik duvarı uyarı ekranı çıkabilir. Bu durum diğer işletim sistemlerinde görülmeyebilir. Eğer bir uyarı alırsanız, "İzin Ver" seçeneğini tıklayarak ilgili portun WekaServer tarafından kullanılmasına izin vermelisiniz. 

**Linux için**, ilgili portun izinli olduğundan emin olmak için aşağıdaki adımları izleyin:
1. Güvenlik duvarı durumunu kontrol edin:
   ```bash
   sudo ufw status
   ```
2. Eğer port izinli değilse, aşağıdaki komutla izin verin:
   ```bash
   sudo ufw allow <PORT_NUMARASI>/tcp
   ```
3. Değişiklikleri doğrulamak için güvenlik duvarı durumunu tekrar kontrol edin:
   ```bash
   sudo ufw status
   ```
4. Gerekirse güvenlik duvarını yeniden başlatın:
   ```bash
   sudo ufw reload
   ```

**Örnek Komut (IP üzerinden, Windows):**

```bash
java -classpath "C:\Program Files\Weka-3-8-6\weka.jar" -Xmx8G -Djava.awt.headless=true weka.Run WekaServer -host 10.222.18.190 -port 8085 -load-adjust 4 -slots 1 -staleTime -1
```

---

## 3. Slave Makineleri Master'a Bağlama

Slave makineler, master'dan aldıkları görevleri işleyen makinelerdir. Her bir slave, kendi komut ekranına (cmd) komut girilerek işlem olarak başlatılır. Slave'lerin doğru şekilde çalışabilmesi için, her birinin master server ile iletişim kurabilmesi adına doğru IP adresi ve port ayarlarıyla yapılandırıldığından emin olunmalıdır. Bu, bağlantı sorunlarını önlemek ve sistemin düzgün çalışmasını sağlamak için kritik bir adımdır.

**Genel Komut Yapısı:**

```bash
java -cp <WEKA_JAR_YOLU> -Xmx<MAKS_BELLEK> -Djava.awt.headless=true weka.Run WekaServer \
  -host <SLAVE_IP_ADRESİ> -port <SLAVE_PORTU> \
  -master <MASTER_IP>:<MASTER_PORTU> -slots <SLOT_SAYISI> -staleTime -1
```

**Parametre Açıklamaları:**

- `-host <SLAVE_IP_ADRESİ>`: Bu slave'in IP adresi.
- `-port <SLAVE_PORTU>`: Bu slave'in dinleyeceği port.
- `-master <MASTER_IP>:<MASTER_PORTU>`: Master adresi ve portu.
- `-slots <SLOT_SAYISI>`: Bu makinenin eş zamanlı çalıştırabileceği görev sayısı.

**Örnek Komut (Windows, 6GB RAM, 4 slot):**

```bash
java -Djava.awt.headless=true -classpath "C:\Program Files\Weka-3-8-6\weka.jar" -Xmx6G weka.Run WekaServer -host 10.202.17.35 -port 8085 -master 10.202.16.171:8085 -slots 4 -staleTime -1
```

---

Bu kurulum sayesinde, Weka'nın dağıtık işlem altyapısını kullanarak büyük ölçekli veri işlemlerini birden fazla makineye dağıtabilir ve işlem süresini önemli ölçüde kısaltabilirsiniz. Denemeler yapalım arkadaşlar.
