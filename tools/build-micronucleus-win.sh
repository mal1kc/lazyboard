#!/usr/bin/env sh
# Cross-build micronucleus.exe + libusb0.dll for 64-bit Windows on Linux,
# from open source code only (micronucleus: GPL, libusb-win32: LGPL).
# Needs: git, make, mingw-w64  (Debian/Ubuntu: sudo apt install git make mingw-w64)
# Output: tools/win/micronucleus.exe and tools/win/libusb0.dll
set -eu

HOST=x86_64-w64-mingw32
ROOT=$(cd "$(dirname "$0")/.." && pwd)
OUT="$ROOT/tools/win"
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

command -v "$HOST-gcc" >/dev/null || { echo "missing $HOST-gcc (install mingw-w64)"; exit 1; }

git clone -q --depth 1 https://github.com/mcuee/libusb-win32 "$WORK/libusb-win32"
git clone -q --depth 1 https://github.com/micronucleus/micronucleus "$WORK/micronucleus"

L="$WORK/libusb-win32/libusb"
# CC must be set: the Makefile would otherwise look for "$HOST-cc", which mingw doesn't ship
make -s -C "$L" host_prefix="$HOST" CC="$HOST-gcc" dll

M="$WORK/micronucleus/commandline"
"$HOST-gcc" -O2 -D WIN -I"$M/library" -I"$L/src" \
  "$M/micronucleus.c" "$M/library/micronucleus_lib.c" "$M/library/littleWire_util.c" \
  -o "$WORK/micronucleus.exe" -L"$L" -lusb -static-libgcc

mkdir -p "$OUT"
cp "$L/libusb0.dll" "$WORK/micronucleus.exe" "$OUT/"
{
  echo "libusb-win32 $(git -C "$WORK/libusb-win32" rev-parse --short HEAD)"
  echo "micronucleus $(git -C "$WORK/micronucleus" rev-parse --short HEAD)"
} > "$OUT/VERSIONS.txt"
echo "built into $OUT:"; ls -l "$OUT"
