#!/bin/bash

if [ "$1" = "" ]; then
	mkdir -p ../build
	pushd ../build
	SRC_DIR="../src"
else
	SRC_DIR="$1"
fi

LIB_LINKER_FLAGS="-lxml2 -lz -framework SDL2 -framework SDL2_image"

DEBUG_APP_LINKER_FLAGS="-framework SDL2"

RELEASE_APP_LINKER_FLAGS="-L. -lGame -framework SDL2"

COMMON_FLAGS="-std=c++14 -I/usr/include/libxml2"

WARN_FLAGS="-Wall -Werror -Wno-unused-variable -Wno-unused-function -Wno-unused-private-field"

DEBUG_FLAGS="${COMMON_FLAGS} ${WARN_FLAGS} -g -DDEBUG -DBUILD_INTERNAL=1 -DBUILD_SLOW=1 -DBUILD_OSX=1"

RELEASE_FLAGS="${COMMON_FLAGS} ${WARN_FLAGS} -Ofast -DNDEBUG -DBUILD_OSX=1"

c++ ${DEBUG_FLAGS} -c ${SRC_DIR}/Game.cpp -o Game.o
c++ ${DEBUG_FLAGS} ${LIB_LINKER_FLAGS} -dynamiclib -o libGame.dylib Game.o
c++ ${DEBUG_FLAGS} ${DEBUG_APP_LINKER_FLAGS} ${SRC_DIR}/main.cpp -o Hello2SDL

#c++ ${RELEASE_FLAGS} -c ${SRC_DIR}/Game.cpp -o Game.o
#c++ ${RELEASE_FLAGS} ${LIB_LINKER_FLAGS} -dynamiclib -o libGame.dylib Game.o
#c++ ${RELEASE_FLAGS} ${RELEASE_APP_LINKER_FLAGS} ${SRC_DIR}/main.cpp -o Hello2SDL

if [ "$1" = "" ]; then
	popd
fi
