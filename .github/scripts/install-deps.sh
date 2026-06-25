#!/usr/bin/env bash
# Install native build dependencies on Debian/Ubuntu CI runners.
set -euo pipefail

profile="${1:-build}"

packages=(
  autoconf
  automake
  libtool
  pkg-config
  gcc
  make
  libncurses-dev
  check
)

case "$profile" in
  build) ;;
  package)
    packages+=(gzip)
    ;;
  *)
    echo "usage: $0 [build|package]" >&2
    exit 2
    ;;
esac

sudo apt-get update
sudo apt-get install -y --no-install-recommends "${packages[@]}"

if [ "$profile" = "package" ]; then
  nfpm_version="v2.47.0"
  curl -fsSL "https://github.com/goreleaser/nfpm/releases/download/${nfpm_version}/nfpm_${nfpm_version#v}_Linux_x86_64.tar.gz" \
    | sudo tar -xz -C /usr/local/bin nfpm
  nfpm --version
fi
