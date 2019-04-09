#QQMusic qmc0 qmcflac QMC-DECODER#

转化QQ Music QMC to MP3 or FLAC

* build

```
mkdir build
cd build
cmake ..
make 
```

* convert

```
cd build
./decoder <file1> <file2> ..... 
```

* batch convert

```
find <qmc file dir> -type f -name "*.qmc*" -print0 | xargs -0 ./decoder
```
