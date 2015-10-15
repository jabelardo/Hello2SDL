#!/bin/bash

if [ "$1" = "" ]; then
	mkdir -p ../build
	pushd ../build
	SRC_DIR="../src"
else
	SRC_DIR="$1"
fi

CommonFlags="-Wall -Werror -Wno-unused-variable -Wno-unused-function \
	-std=c++14 -DBUILD_INTERNAL=1 -DBUILD_SLOW=1 \
	-DBUILD_OSX=1 -framework SDL2 -framework SDL2_image"

c++ $CommonFlags ${SRC_DIR}/main.cpp -o Hello2SDL -g

if [ "$1" = "" ]; then
	popd
fi
