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