#!/usr/bin/env bash
# Build Linux packages (.deb, .rpm, .apk) from a staged Autotools install.
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"

version="${NFPM_VERSION:-}"
if [ -z "$version" ]; then
  version="$(sed -n 's/^AC_INIT(\[[^]]*\], \[\([^]]*\)\].*/\1/p' configure.ac)"
fi
if [ -z "$version" ]; then
  echo "error: could not determine package version" >&2
  exit 1
fi

formats=("$@")
if [ "${#formats[@]}" -eq 0 ]; then
  formats=(deb rpm apk)
fi

staging="$root/packaging/staging"
output="$root/packaging/dist"
rm -rf "$staging" "$output"
mkdir -p "$output"

./bootstrap
./configure --prefix=/usr
make -j"$(nproc)"
make install DESTDIR="$staging"

export NFPM_VERSION="$version"
export NFPM_ARCH="${NFPM_ARCH:-amd64}"

for format in "${formats[@]}"; do
  nfpm package \
    --config "$root/packaging/nfpm.yaml" \
    --packager "$format" \
    --target "$output"
done

echo "Packages written to $output:"
ls -1 "$output"
