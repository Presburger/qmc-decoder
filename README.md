# QQMusic QMC Decoder (convert QMC File to MP3 or FLAC)

[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg?style=flat-square)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-MIT-red.svg?style=flat-square)](https://github.com/Presburger/qmc-decoder/blob/master/LICENSE)


***SUPPORT QMC3/QMC0/QMCFLAC, Faster***

## Release

binary executable file is released [release](https://github.com/Presburger/qmc-decoder/releases)

## Build

* for linux

```
* Note: additional components are required to provide build support:
debian: sudo dnf install cmake libboost-all-dev
fedora: sudo apt-get install cmake boost-devel libstdc++-static
```

```shell
mkdir build
cd build
cmake ..
make
```

* for macOS
```shell
brew install cmake
mkdir build && cd build
cmake ..
make
```



* for windows

```bat
mkdir build
cd build
cmake -G "NMake Makefiles" ..
nmake
```

## Convert

Put the execuatable file in your qmc file directory, then run the execuatable file.

For windows user, just click the `decoder.exe` when you put the `decoder.exe` into your qmc file directory, it will convert all qmc file automatically.

![eXlSt1.gif](https://s2.ax1x.com/2019/08/10/eXlSt1.gif)

For mac user, double-click the decoder.command file, before you need to put the decoder.command and decoder files in the qmc music file directory.

![EjHn9U.gif](https://s2.ax1x.com/2019/05/19/EjHn9U.gif)


* Todo

support auto fetch albums

support auto fix music meta data

