1- MSYS2 yüklenir.
2- MSYS2 konsola "pacman -Syu" yazılarak paketler güncellenir.
3- "pacman -S mingw-w64-x86_64-gcc" ile gcc derleyici yüklenir.
4- Windows Ortam değişkenlerine gcc path değeri aşağıdaki gibi eklenir:
c:\msys64\mingw64\bin
5- GCC nin sistem tarafından doğrulamak için cmd ekranına "gcc --version" yazılır. GCC version bilgisi geliyorsa her şey yolundadır. Gelmiyorsa önceki adımlara dönülmelidir.
6- C kodu yazılır ve bir isimle kaydedilir. örneğin "openmp_deneme.c"
7- .c dosyasının klasörüne konsoldan gidilir ve derleme için şu kod yazılır:
"gcc -fopenmp  -o  openmp_exe_adi   openmp_deneme.c"
8- Hata almadıysak. Artık exe olarak oluşan openmp_exe_adi dosyasını konsola giderek çalıştırabiliriz.
Konsola:
".\openmp_exe_adi" yazmak ve Enter yapmak yeterli olacaktır.