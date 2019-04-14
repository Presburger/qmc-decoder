# QQMusic qmc0 qmcflac Decoder

转化QQ Music QMC to MP3 or FLAC

* binary executable file for mac and linux is prepared in release

[release](https://github.com/Presburger/qmc-decoder/releases)

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
