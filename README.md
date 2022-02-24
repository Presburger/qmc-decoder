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

Run the following command from terminal:
```bash
qmc-decoder /PATH/TO/SONG
```


* Todo

support auto fetch albums

support auto fix music meta data
