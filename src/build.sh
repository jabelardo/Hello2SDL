#!/bin/bash

if [ "$1" = "" ]; then
	mkdir -p ../build
	pushd ../build
	SRC_DIR="../src"
else
	SRC_DIR="$1"
fi

CommonFlags="-Wall -Werror -Wno-unused-variable -Wno-unused-function -Wno-unused-private-field \
	-std=c++14 -DBUILD_INTERNAL=1 -DBUILD_SLOW=1 -DBUILD_OSX=1 \
	-DTMXEXPORT= -DWANT_XML=1 -I/usr/include/libxml2 -lxml2 -DWANT_ZLIB=1 -lz
  -framework SDL2 -framework SDL2_image"

c++ $CommonFlags ${SRC_DIR}/main.cpp -o Hello2SDL -g

if [ "$1" = "" ]; then
	popd
fi
