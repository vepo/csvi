# CSVI

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=csv-viewer&metric=alert_status)](https://sonarcloud.io/dashboard?id=csv-viewer)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=csv-viewer&metric=bugs)](https://sonarcloud.io/dashboard?id=csv-viewer)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=csv-viewer&metric=code_smells)](https://sonarcloud.io/dashboard?id=csv-viewer)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=csv-viewer&metric=coverage)](https://sonarcloud.io/dashboard?id=csv-viewer)

CSV Viewer for the Linux command line.

## Install

```bash
curl https://vepo.github.io/assets/bin/get-csvi | sudo bash
```

## Build

```bash
sudo apt update
sudo apt install gcc libtool make check pkg-config libncurses5-dev -y

autoreconf -ivf
./configure
make
make check
sudo make install   # installs csvi and man/csvi.1
```

## Usage

```bash
csvi [options] file.csv
csvi -V data.csv          # verbose logging to stderr
csvi -s , data.csv        # comma-separated
csvi -h                   # help
```

See [Commands.md](./Commands.md) for key bindings and `:` commands.  
See [docs/architecture.md](./docs/architecture.md) for module layout.

## Shell completion

```bash
# bash
sudo cp completions/csvi.bash /etc/bash_completion.d/csvi

# zsh
sudo cp completions/csvi.zsh /usr/share/zsh/site-functions/_csvi
```

## Code coverage

```bash
./conf-clean
./conf-gen
./configure --enable-coverage
make check
lcov -c --directory src --directory tests --output-file coverage.info
genhtml coverage.info --output-directory coverage-report
```

## Contributing

See [CONTRIBUTING.md](./CONTRIBUTING.md) and [AGENTS.md](./AGENTS.md).
