#!/bin/bash

cd $(dirname $0)

basedir=$(pwd)

mkdir usr

wget https://dl.bintray.com/boostorg/release/1.72.0/source/boost_1_72_0.tar.gz

tar -zxvf boost_1_72_0.tar.gz

cd boost_1_72_0


./bootstrap.sh --with-toolset=gcc --without-icu --with-libraries=filesystem \
--prefix=$basedir/usr 

ismingw="$1"

if [ "$ismingw" == "true" ];then
    sed -i 's/using gcc ;/using gcc :  : x86_64-w64-mingw32-g++ ;/g' project-config.jam 
fi

./b2  variant=release \
      debug-symbols=off \
      link=static \
      cflags="${CPPFLAGS} ${CFLAGS} -fPIC -O2" \
      cxxflags="${CPPFLAGS} ${CXXFLAGS} -std=c++11 -fPIC -O2" \
      --with-filesystem \
      install 

cd ..
rm -rf boost_1_72_0*


