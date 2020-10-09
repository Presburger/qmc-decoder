# QQMusic QMC Decoder (convert QMC File to MP3 or FLAC)

[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg?style=flat-square)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-MIT-red.svg?style=flat-square)](https://github.com/Presburger/qmc-decoder/blob/master/LICENSE)

![Build Binary](https://github.com/Presburger/qmc-decoder/workflows/Build%20Binary/badge.svg)

***SUPPORT QMC3/QMC0/QMCFLAC, Faster***

### MUSIC FILE TAG FIX, CAN TRY THIS SOFTWARE 
[Tag Editor](https://amvidia.com/tag-editor)
## Release

binary executable file is released [release](https://github.com/Presburger/qmc-decoder/releases)

## Build

* for linux

```shell
git clone https://github.com/Presburger/qmc-decoder.git
cd qmc-decoder
git submodule update --init
mkdir build
cd build
cmake ..
make
```

* for macOS
```shell
# install cmake 
brew install cmake
git clone https://github.com/Presburger/qmc-decoder.git
cd qmc-decoder
git submodule update --init
mkdir build && cd build
cmake ..
make
```

* for windows

```bat
# PowerShell, please install MSVC compiler and Git with environment variables configured
# or in x86 or x64 Native Tools Command Prompt for VS 2019 
git clone https://github.com/Presburger/qmc-decoder.git
cd qmc-decoder
git submodule update --init
mkdir build
cd build
cmake -G "NMake Makefiles" .. -DCMAKE_BUILD_TYPE=Release
nmake
```

## Convert

Put the execuatable file in your qmc file directory, then run the execuatable file.

For mac user, double-click the decoder.command file, before you need to put the `decoder.command` and `decoder` files in the qmc music file directory.

![EjHn9U.gif](https://s2.ax1x.com/2019/05/19/EjHn9U.gif)

For windows user, just click the `decoder-win.exe` when you put the `decoder-win.exe` into your qmc file directory, it will convert all qmc file automatically.

![tW1w7D.gif](https://s1.ax1x.com/2020/06/08/tW1w7D.gif)


* Todo

support auto fetch albums

support auto fix music meta data
