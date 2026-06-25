# CSVI Tutorial

CSVI is a terminal CSV viewer for Linux. It opens a file as a scrollable cell grid: move one cell at a time, jump with commands, and search across all values. This guide walks through everyday use from first launch to search and customization.

For a quick reference, see [Commands.md](../Commands.md). For module layout and internals, see [architecture.md](./architecture.md).

## Quick start

Install a prebuilt binary:

```bash
curl https://vepo.github.io/assets/bin/get-csvi | sudo bash
```

Or build from source (see [README.md](../README.md)). Then open any CSV file:

```bash
csvi data.csv
```

By default CSVI expects semicolon-separated values (`;`). For comma-separated files:

```bash
csvi -s , data.csv
```

Press `q` or `:q` to quit.

## The screen

When CSVI starts you see:

- **Grid** — one screenful of cells. With color enabled (default `auto`), adjacent cells alternate light/dark backgrounds so columns and rows are easy to scan; the selected cell is highlighted in green. Without color, the selected cell uses reverse video.
- **Status bar** — bottom line with current row/column, mode, and messages.

Example status text in normal mode:

```text
row 42/1000 · col 3/12
```

In search mode you also see match position, e.g. `match 2/15`.

Press `?` anytime for an on-screen help overlay. Press `Esc` to close it.

## Moving around

Navigation is **cell-based**: each key moves the cursor one cell, not one line of text.

| Key | Action |
|-----|--------|
| `↑` `↓` `←` `→` or `h` `j` `k` `l` | Move one cell |
| `Home` / `End` | First / last column in the current row |
| `g` / `G` | First / last row |
| `Ctrl+F` / `Page Down` | Page down |
| `Ctrl+B` / `Page Up` | Page up |
| `Ctrl+H` / `Ctrl+L` | Page left / right (horizontal scroll) |
| `Enter` | Show the full cell value in the status bar (useful when text is truncated) |

The viewport scrolls automatically so the selected cell stays visible.

## Command mode

Press `:` to enter command mode. The status bar shows your input. Press `Enter` to run the command, or `Esc` to cancel.

Row and column numbers are **1-based** (row 1 is the first line in the file).

| Command | Action |
|---------|--------|
| `:q` or `:quit` | Exit |
| `:line N` or `:N` | Go to row N |
| `:col N` | Go to column N |
| `:cell ROW,COL` or `:ROW,COL` | Go to a specific cell (e.g. `:cell 42,3`) |
| `:top` / `:bottom` | First / last row |
| `:left` / `:right` | First / last column |
| `:n` / `:next` | Next search match |
| `:N` / `:prev` | Previous search match |
| `:set sep=X` | Change separator (reload the file to apply) |
| `:set header=on\|off` | Toggle frozen header row |

### Example workflow

Inspect row 500, column 7:

```text
:cell 500,7
```

Jump to the header row, then scan columns:

```text
:top
```

Use `:col 5` or arrow keys to move across.

## Search

1. Press `/` to enter search mode.
2. Type a substring and press `Enter`.
3. Matching cells are highlighted; the status bar shows `match i/n`.
4. Press `n` for the next match, `N` for the previous one (works in normal mode after the search runs).

You can also use `:n` and `:N` from command mode.

Search is a simple substring match over all cell values. Press `Esc` while typing to cancel search input.

## Command-line options

| Flag | Description |
|------|-------------|
| `-s`, `--separator=CHAR` | Cell separator (default `;`) |
| `--color=auto\|never\|always` | Color mode (default `auto`; honors `NO_COLOR`) |
| `--grid` | Draw `\|` column separators |
| `--header` | Freeze row 1 as a header while scrolling |
| `-V`, `--verbose` | Log diagnostics to stderr |
| `-h`, `--help` | Print usage |
| `-v`, `--version` | Print version |

Examples:

```bash
csvi --header --grid -s , sales.csv
csvi --color=never large-export.csv
NO_COLOR=1 csvi report.csv    # same effect as --color=never when auto
```

## Tips

**Header row.** Files with column names on line 1 work well with `--header` or `:set header=on`. The first row stays visible while you scroll.

**Wide tables.** Use `Ctrl+L` / `Ctrl+H` to scroll horizontally when there are many columns.

**Wrong separator.** If cells look like one long line, pass `-s ,` (or tab: `-s $'\t'`) on the command line.

**Reload after `:set sep=`.** Changing the separator in command mode does not re-parse the open file. Quit and reopen with the new separator, or restart `csvi` with `-s`.

**Shell completion.** Install bash or zsh completions from the `completions/` directory (see [README.md](../README.md)).

## Exit codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | General error |
| 2 | Usage error (missing file, bad flags) |
| 3 | I/O error (file not found or unreadable) |

## Migrating from older CSVI

If you used an older build, some bindings changed:

| Legacy | New |
|--------|-----|
| `Esc` → command | `:` |
| `:c3` | `:col 3` |
| `:3x42` | `:cell 42,3` |
| `:0` / `:-1` | `:top` / `:bottom` |
| `:c0` / `:c-1` | `:left` / `:right` |
| `Home` / `End` (horizontal page) | `Ctrl+H` / `Ctrl+L` |

See [Commands.md](../Commands.md) for the full migration table.

## Next steps

- [Commands.md](../Commands.md) — complete key map and command reference
- [man/csvi.1](../man/csvi.1) — installed manual page (`man csvi`)
- [architecture.md](./architecture.md) — how CSVI is structured (for contributors)
