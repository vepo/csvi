# CSVI — Agent Guide

CSVI is a terminal CSV viewer for Linux (`csvi` binary), built in C with ncurses and Autotools.

## Build and test

```bash
./bootstrap
./configure
make
make check
```

Coverage build:

```bash
./configure --enable-coverage
make check
```

Full instructions: [docs/build.md](docs/build.md).

## Architecture

Read **[docs/architecture.md](docs/architecture.md)** before changing module boundaries, data flow, or the CLI surface. Update that document in the same change when you alter structure.

## Source layout

| Path | Role |
|------|------|
| `src/bin/main.c` | CLI parsing, exit codes |
| `src/lib/app/` | Viewer state, paint strategy, search |
| `src/lib/io/` | File I/O and CSV parsing |
| `src/lib/layout/` | Cell sizing, viewport cache, shared types |
| `src/lib/ui/` | ncurses presentation, status bar, input modes |
| `src/lib/nav/` | Cursor/viewport navigation |
| `src/lib/cmd/` | Cell-address `:` command parsing (`cell-commands.c`) |
| `src/lib/common/` | Exit codes, logging, helpers |
| `tests/` | Check-based unit tests |

## Conventions

- **C symbols**: snake_case
- **Filenames**: kebab-case (`csv-reader.c`)
- **Includes**: `-I src/lib`, paths like `#include "io/csv-reader.h"`
- **Coordinates**: width → x, height → y (CSV line index uses y)
- **Library code**: no `exit()`; return error codes or NULL
- **Tests**: required for non-UI logic changes; run `make check`

## User docs

- [Commands.md](Commands.md) — key bindings and `:` commands
- [man/csvi.1](man/csvi.1) — installed man page
