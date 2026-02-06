#!/usr/bin/env bash

set -ex

SRC="msdfgen"
[ -d "$SRC" ] || git clone https://github.com/Chlumsky/msdfgen "$SRC" --depth=1

pushd "$SRC"
BIN="build"
# vcpkg install --x-install-root="$BIN/vcpkg_installed"
emcmake cmake . -B "$BIN" -DCMAKE_BUILD_TYPE=Release -DMSDFGEN_INSTALL=ON \
    -DMSDFGEN_DISABLE_SVG=1 -DMSDFGEN_DISABLE_PNG=1 -DMSDFGEN_USE_SKIA=OFF \
    -DFREETYPE_LIBRARY=$(pwd)/../../ft/freetype.wasm.a -DFREETYPE_INCLUDE_DIRS=$(pwd)/../../ft/freetype/include #-DBUILD_SHARED_LIBS=ON
# -DCMAKE_FIND_ROOT_PATH_BOTH=ON
#-DVCPKG_TARGET_TRIPLET=wasm32-emscripten -DCMAKE_TOOLCHAIN_FILE=/opt/homebrew/Cellar/emscripten/4.0.23/libexec/cmake/Modules/Platform/Emscripten.cmake

if [ $(uname -s) = 'Darwin' ]; then
    NCORE=$(sysctl -n hw.ncpu)
else
    NCORE=$(nproc)
fi

echo "Building project..."
emmake make -C $BIN -j$NCORE
cp "$BIN"/msdfgen-config.h .
popd

echo "Building static lib..."
em++ -c msdfgen-c/msdfgen-core.cpp -I. -I"$SRC" -o core.wasm.o
em++ -c msdfgen-c/msdfgen-ext.cpp -I. -I"$SRC" -o ext.wasm.o

# Extract .o files from archives and combine into single archive
TMPDIR=$(mktemp -d)
pushd "$TMPDIR"
emar x "$OLDPWD/$SRC/$BIN/libmsdfgen-core.a"
emar x "$OLDPWD/$SRC/$BIN/libmsdfgen-ext.a"
popd
emar rcs msdf.wasm.a "$TMPDIR"/*.o core.wasm.o ext.wasm.o
rm -rf "$TMPDIR" core.wasm.o ext.wasm.o
