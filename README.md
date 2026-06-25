# CSVI

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=csv-viewer&metric=alert_status)](https://sonarcloud.io/dashboard?id=csv-viewer)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=csv-viewer&metric=bugs)](https://sonarcloud.io/dashboard?id=csv-viewer)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=csv-viewer&metric=code_smells)](https://sonarcloud.io/dashboard?id=csv-viewer)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=csv-viewer&metric=coverage)](https://sonarcloud.io/dashboard?id=csv-viewer)

CSV Viewer for the Linux command line. Navigate by cell with arrow keys or `hjkl`; use `:` for commands and `/` for search. A status bar shows row, column, and mode.

## Install

Prebuilt packages are attached to [GitHub releases](https://github.com/vepo/csvi/releases) (`.deb`, `.rpm`, `.apk`) or install via the shell script:

```bash
curl https://vepo.github.io/assets/bin/get-csvi | sudo bash
```

## Build

See **[docs/build.md](docs/build.md)** for prerequisites, configure options, out-of-tree builds, coverage, and troubleshooting.

```bash
./bootstrap
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
csvi --color=never --grid data.csv
csvi --header data.csv    # freeze first row
csvi -h                   # help
```

**Breaking change:** legacy `:cN`, `:-1`, `Esc`-as-command, and `Home`/`End` as horizontal page keys are removed. See [Commands.md](./Commands.md) for the new key map and migration table.

See [docs/tutorial.md](./docs/tutorial.md) for a getting-started guide.  
See [docs/build.md](./docs/build.md) for build instructions.  
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
./configure --enable-coverage
make check
lcov -c --directory src --directory tests --output-file coverage.info
genhtml coverage.info --output-directory coverage-report
```

See [docs/build.md](docs/build.md) for details.

## Contributing

See [CONTRIBUTING.md](./CONTRIBUTING.md) and [AGENTS.md](./AGENTS.md).
