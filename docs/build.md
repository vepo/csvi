# Building CSVI

CSVI uses the GNU Autotools chain (Autoconf, Automake, Libtool). Generated files such as `configure` and `Makefile.in` are **not** committed to the repository — run `./bootstrap` once after cloning, then the usual `./configure && make` workflow.

## Prerequisites

### Debian / Ubuntu

```bash
sudo apt update
sudo apt install -y \
  autoconf automake libtool pkg-config \
  gcc make \
  libncurses-dev \
  check
```

On older distributions the ncurses package may be named `libncurses5-dev` or `libncursesw5-dev`.

### Fedora / RHEL

```bash
sudo dnf install -y \
  autoconf automake libtool pkg-config \
  gcc make \
  ncurses-devel \
  check-devel
```

### Optional (code coverage reports)

```bash
sudo apt install -y lcov    # Debian / Ubuntu
```

## Quick build

From a fresh clone:

```bash
./bootstrap          # generates configure and Makefile.in
./configure
make
make check           # run the unit test suite
```

The `csvi` binary is built at `src/bin/csvi`. Run it directly or install system-wide:

```bash
./src/bin/csvi inputs/example-1.csv
sudo make install    # installs csvi and man/csvi.1
```

## Configure options

| Flag | Default | Description |
|------|---------|-------------|
| `--prefix=PATH` | `/usr/local` | Installation prefix |
| `--enable-debug` | no | Add `-g` debug symbols |
| `--enable-coverage` | no | Enable gcov instrumentation (`-fprofile-arcs -ftest-coverage`) |

Examples:

```bash
./configure --enable-debug
./configure --prefix=$HOME/.local
./configure --enable-coverage
```

## Common targets

| Target | Description |
|--------|-------------|
| `make` | Build the `csvi` binary and test programs |
| `make check` | Build and run all unit tests |
| `make install` | Install binary and man page under `$(prefix)` |
| `make dist` | Create `csv-viewer-VERSION.tar.gz` source tarball |
| `make distcheck` | Build, test, and verify the tarball in a clean tree |
| `make distclean` | Remove build products and generated Makefiles |
| `make clean` | Remove object files and binaries (keep Makefiles) |

## Out-of-tree build

Building in a separate directory keeps the source tree clean:

```bash
./bootstrap
mkdir -p build && cd build
../configure
make
make check
```

## Code coverage

```bash
./configure --enable-coverage
make check
lcov -c --directory src --directory tests --output-file coverage.info
genhtml coverage.info --output-directory coverage-report
```

Open `coverage-report/index.html` in a browser. CI uploads gcov data to SonarCloud for continuous coverage tracking.

## Cleaning up

| Goal | Command |
|------|---------|
| Remove build artifacts | `make distclean` |
| Regenerate Autotools files | `./bootstrap` |
| Return to pristine checkout | `make distclean && git clean -fdx` |

The last command removes all untracked and ignored files (including `configure`). Run `./bootstrap` again before the next build.

## Source tarball and installer

Release tarballs are produced with:

```bash
./bootstrap
./configure
make dist
```

The self-extracting installer script used on the project website is built with:

```bash
./build-installer
```

This runs `make dist`, embeds the tarball in `installer-template`, and writes `installer`.

## Troubleshooting

**`configure: not found`** — Run `./bootstrap` first.

**`Package 'check' not found`** — Install the Check unit-test framework (`check` on Debian, `check-devel` on Fedora).

**`Package 'ncurses' not found`** — Install ncurses development headers (`libncurses-dev` or equivalent).

**Tests fail after switching configure flags** — Run `make distclean`, re-run `./configure` with the desired flags, then `make check`.

**Stale Autotools warnings** — After editing `configure.ac` or any `Makefile.am`, run `./bootstrap` and commit only the source files (not the generated `configure` or `Makefile.in`).

## Project layout (build-related)

| Path | Role |
|------|------|
| `configure.ac` | Autoconf macros, feature checks, `configure` options |
| `Makefile.am` | Top-level Automake rules (subdirs, man page, dist files) |
| `src/lib/Makefile.am` | Static libraries (`libcsvviewer-core.a`, `libcsvviewer-base.a`) |
| `src/bin/Makefile.am` | `csvi` binary |
| `tests/Makefile.am` | Check-based unit tests |
| `bootstrap` | Regenerates the Autotools build system |
| `build-installer` | Packages `make dist` output into the web installer |

For module architecture and coding conventions, see [architecture.md](./architecture.md) and [AGENTS.md](../AGENTS.md).
