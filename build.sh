#!/usr/bin/env bash

set -ex

BASE="$(cd "$(dirname "$0")" && pwd)"
SRC="msdfgen"
cd "$BASE"
clone_at_revision() {
    local dir="$1"
    local revision="$2"
    local remote="$3"
    shift 3
    [ -d "$dir" ] && return
    git clone "$@" "$remote" "$dir"
    if ! git -C "$dir" checkout --detach "$revision"; then
        git -C "$dir" fetch origin "$revision"
        git -C "$dir" checkout --detach FETCH_HEAD
    fi
    if [ -f "$dir/.gitmodules" ]; then
        git -C "$dir" submodule update --init --recursive
    fi
}

clone_at_revision "$SRC" 1874bcf7d9624ccc85b4bc9a85d78116f690f35b https://github.com/Chlumsky/msdfgen --depth=1

linux_arch_dir() {
    case "$(uname -m)" in
        x86_64 | amd64) echo "linux_x64" ;;
        aarch64 | arm64) echo "linux_arm64" ;;
        *) echo "linux_$(uname -m)" ;;
    esac
}

# Apply local patches (safe to run repeatedly)
for patch in patches/*.patch; do
    [ -f "$patch" ] && git -C "$SRC" am --3way "$PWD/$patch" 2>/dev/null || true
done

pushd "$SRC"
BIN="build"

CMAKE_FLAGS=(-DMSDFGEN_INSTALL=ON -DCMAKE_POSITION_INDEPENDENT_CODE=ON)
if [ "$(uname -s)" = 'Linux' ]; then
    CMAKE_FLAGS+=(-DMSDFGEN_USE_SKIA=OFF)
fi
cmake . -B "$BIN" -DCMAKE_BUILD_TYPE=Release "${CMAKE_FLAGS[@]}" #-DBUILD_SHARED_LIBS=ON

if [ $(uname -s) = 'Darwin' ]; then
    NCORE=$(sysctl -n hw.ncpu)
    LIB_EXT=darwin
    DLL_EXT=dylib
    SHARED_OUT="$BASE/msdf.$DLL_EXT"
    STATIC_OUT="$BASE/msdf.$LIB_EXT.a"
else
    NCORE=$(nproc)
    LIB_EXT=linux
    DLL_EXT=so
    ARCH_DIR=$(linux_arch_dir)
    OUT_DIR="$BASE/$ARCH_DIR"
    mkdir -p "$OUT_DIR"
    SHARED_OUT="$OUT_DIR/msdf.$DLL_EXT"
    STATIC_OUT="$OUT_DIR/msdf.$LIB_EXT.a"
fi

echo "Building project..."
cmake --build "$BIN" --config Release -j$NCORE
cp "$BIN"/msdfgen-config.h .
# cp "$BIN"/*."$DLL_EXT" ..
for f in "$BIN/"vcpkg_installed/**; do
    [ -d "$f/lib" ] && VCPKG_LIB="$SRC/$f"
done
popd

STATIC_DEPS=(libbrotlicommon.a libbrotlidec.a libbz2.a libfreetype.a libpng16.a libtinyxml2.a libz.a)
if [ -f "$VCPKG_LIB/lib/libskia.a" ]; then
    STATIC_DEPS+=(libskia.a)
fi
STATIC_DEPS=("${STATIC_DEPS[@]/#/$VCPKG_LIB/lib/}")

echo "Building shared lib..."
clang++ msdfgen-c/msdfgen-core.cpp msdfgen-c/msdfgen-ext.cpp -I. \
    "$SRC/$BIN/libmsdfgen-core.a" "$SRC/$BIN/libmsdfgen-ext.a" "${STATIC_DEPS[@]}" \
    -shared -fPIC -o "$SHARED_OUT"

echo "Building static lib..."
clang++ -c msdfgen-c/msdfgen-core.cpp -I. -I"$SRC" -o core.o
clang++ -c msdfgen-c/msdfgen-ext.cpp -I. -I"$SRC" -o ext.o
if [ "$(uname -s)" = 'Darwin' ]; then
    libtool -static -o "$STATIC_OUT" "$SRC/$BIN/libmsdfgen-core.a" "$SRC/$BIN/libmsdfgen-ext.a" "${STATIC_DEPS[@]}" core.o ext.o
else
    ar rcs "$STATIC_OUT" "$SRC/$BIN/libmsdfgen-core.a" "$SRC/$BIN/libmsdfgen-ext.a" "${STATIC_DEPS[@]}" core.o ext.o
    ranlib "$STATIC_OUT"
fi
rm core.o ext.o
