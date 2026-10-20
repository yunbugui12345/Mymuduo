#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"

# 建议用 /usr/local，若坚持原路径可改回 /usr/include 和 /usr/lib
INCLUDE_DIR="/usr/local/include/muduo_mine"
LIB_DIR="/usr/local/lib"

if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
else
    SUDO="sudo"
fi

# 构建
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ..
make -j"$(nproc)"

# 安装头文件
$SUDO mkdir -p "$INCLUDE_DIR"
for header in "$ROOT_DIR"/*.h; do
    [ -e "$header" ] || continue
    $SUDO cp -f "$header" "$INCLUDE_DIR"/
done

# 安装动态库，按你的 CMake 实际输出路径调整
LIB_SO="$ROOT_DIR/lib/libmuduo_mine.so"
if [ ! -f "$LIB_SO" ]; then
    LIB_SO="$BUILD_DIR/lib/libmuduo_mine.so"
fi
if [ ! -f "$LIB_SO" ]; then
    echo "找不到 libmuduo_mine.so，请检查 CMake 输出路径" >&2
    exit 1
fi

$SUDO cp -f "$LIB_SO" "$LIB_DIR"/
$SUDO ldconfig