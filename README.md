# QQMusic QMC Decoder

## convert QMC File to MP3 or FLAC

**SUPPORT QMC3/QMC0/QMCFLAC**

* binary executable file for windows, mac and linux is prepared in 
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
* todo

find a useful xargs script works in windows,to support batch convert in windows

support auto fetch albums

support auto fix music meta data
