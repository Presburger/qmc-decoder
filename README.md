# QQMusic QMC Decoder (convert QMC File to MP3 or FLAC)

[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg?style=flat-square)](https://github.com/996icu/996.ICU/blob/master/LICENSE)

[![LICENSE](https://img.shields.io/badge/license-MIT-red.svg)]
(https://github.com/Presburger/qmc-decoder/blob/master/LICENSE)

***SUPPORT QMC3/QMC0/QMCFLAC, Faster***

* binary executable file for windows, mac and linux is prepared in 
[release](https://github.com/Presburger/qmc-decoder/releases)


* build

```sh
mkdir build
cd build
cmake ..
make 
```

* convert

```sh
#linux and mac user
cd build
./decoder <file1> <file2> ..... 
#windows user
decoder.exe <file1> <file2> .....
```

* batch convert

```sh
#only for mac and linux user
find <qmc file dir> -type f -name "*.qmc*" -print0 | xargs -0 ./decoder
```
* todo

find a useful xargs script works in windows,to support batch convert in windows

support auto fetch albums

support auto fix music meta data
