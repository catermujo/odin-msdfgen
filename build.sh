#!/usr/bin/env bash

set -ex

SRC="msdfgen"
[ -d "$SRC" ] || git clone https://github.com/Chlumsky/msdfgen "$SRC" --depth=1

pushd "$SRC"
BIN="build"
cmake . -B "$BIN" -DCMAKE_BUILD_TYPE=Release -DMSDFGEN_INSTALL=ON #-DBUILD_SHARED_LIBS=ON

if [ $(uname -s) = 'Darwin' ]; then
    NCORE=$(sysctl -n hw.ncpu)
    LIB_EXT=darwin
    DLL_EXT=dylib
else
    NCORE=$(nproc)
    LIB_EXT=linux
    DLL_EXT=so
fi

echo "Building project..."
cmake --build "$BIN" --config Release -j$NCORE
cp "$BIN"/msdfgen-config.h .
# cp "$BIN"/*."$DLL_EXT" ..
for f in "$BIN/"vcpkg_installed/**; do
    [ -d "$f/lib" ] && VCPKG_LIB="$SRC/$f"
done
popd

STATIC_DEPS=(libbrotlicommon.a libbrotlidec.a libbz2.a libfreetype.a libpng16.a libtinyxml2.a libz.a libskia.a)
STATIC_DEPS=("${STATIC_DEPS[@]/#/$VCPKG_LIB/lib/}")

echo "Building shared lib..."
clang++ msdfgen-c/msdfgen-core.cpp msdfgen-c/msdfgen-ext.cpp -I. \
    "$SRC/$BIN/libmsdfgen-core.a" "$SRC/$BIN/libmsdfgen-ext.a" "${STATIC_DEPS[@]}" \
    -shared -fPIC -o msdf."$DLL_EXT"

echo "Building static lib..."
clang++ -c msdfgen-c/msdfgen-core.cpp -I. -I"$SRC" -o core.o
clang++ -c msdfgen-c/msdfgen-ext.cpp -I. -I"$SRC" -o ext.o
libtool -static -o msdf."$LIB_EXT".a "$SRC/$BIN/libmsdfgen-core.a" "$SRC/$BIN/libmsdfgen-ext.a" "${STATIC_DEPS[@]}" core.o ext.o
rm core.o ext.o
