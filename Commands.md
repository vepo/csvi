# Commands

CSVI is a cell-grid viewer: navigation moves one cell at a time, the status bar shows `row N/M · col X/Y`, and commands use CSV terminology (`:line`, `:col`, `:cell`).

## Key map

| Key | Action |
|-----|--------|
| `↑` `↓` `←` `→` | Move one cell |
| `h` `j` `k` `l` | Move one cell (same as arrows) |
| `Home` | First column in current row |
| `End` | Last column in current row |
| `Ctrl+F` / `Page Down` | Page down |
| `Ctrl+B` / `Page Up` | Page up |
| `g` | First row |
| `G` | Last row |
| `Ctrl+H` | Page left (horizontal scroll) |
| `Ctrl+L` | Page right (horizontal scroll) |
| `Enter` | Echo full cell value in status bar |
| `i` / `Insert` | Enter INSERT mode on current cell |
| `:` | Command mode |
| `/` | Search mode |
| `?` | Help overlay |
| `n` / `N` | Next / previous search match |
| `q` | Quit |
| `Esc` | Cancel command/search; close help; cancel cell edit |

## Editing (INSERT mode)

Press `i` or `Insert` on the selected cell to edit in place. The cell uses a distinct color (cyan when color is enabled). The status bar shows `INSERT` and the live draft.

| Key | INSERT mode |
|-----|-------------|
| Type / Backspace | Edit cell text |
| `←` / `→` | Move cursor within the cell |
| `Enter` | Commit cell to memory (marks file modified) |
| `Esc` | Cancel edit (restore value from when edit started) |
| Navigation keys | Blocked until you commit or cancel |

Committed changes stay in memory until you write the file. The status bar shows `filename*` when the file has unsaved changes.

| Command | Action |
|---------|--------|
| `:w` / `:write` | Write file to disk |
| `:wq` | Write file and quit |
| `:q!` / `:quit!` | Quit without saving |

Read-only files (no write permission) cannot be edited.

## Command mode (`:`)

Commands use **1-based** row and column numbers (internally converted to 0-based).

| Command | Action |
|---------|--------|
| `:q` / `:quit` | Exit (error if file modified) |
| `:w` / `:write` | Write file to disk |
| `:wq` | Write file and quit |
| `:q!` / `:quit!` | Quit without saving |
| `:line N` | Go to row N |
| `:col N` | Go to column N |
| `:cell ROW,COL` | Go to cell (e.g. `:cell 42,3`) |
| `:N` | Shorthand for `:line N` |
| `:ROW,COL` | Shorthand for `:cell ROW,COL` |
| `:top` | First row |
| `:bottom` | Last row |
| `:left` | First column |
| `:right` | Last column |
| `:n` / `:next` | Next search match |
| `:N` / `:prev` | Previous search match |
| `:set sep=X` | Change separator (reload file to apply) |
| `:set header=on\|off` | Toggle frozen header row |

## Search (`/`)

1. Press `/`, type a pattern, press `Enter`.
2. Matching cells are highlighted; status bar shows `match i/n`.
3. `n` / `N` (or `:n` / `:N`) cycle matches.

## CLI flags

| Flag | Description |
|------|-------------|
| `-s`, `--separator=CHAR` | Cell separator (default `;`) |
| `--color=auto\|never\|always` | Color mode (default `auto`; respects `NO_COLOR`). When on, cells use alternating backgrounds; selection is green; editing cell is cyan. |
| `--grid` | Draw `\|` column separators |
| `--header` | Freeze row 0 as header |
| `-V`, `--verbose` | Log diagnostics to stderr |

## Migration from legacy commands

| Legacy | New |
|--------|-----|
| `Esc` → command | `:` |
| `:42` | `:line 42` or `:42` |
| `:c3` | `:col 3` |
| `:3x42` | `:cell 42,3` |
| `:0` / `:-1` | `:top` / `:bottom` |
| `:c0` / `:c-1` | `:left` / `:right` |
| `Home` / `End` (page left/right) | `Ctrl+H` / `Ctrl+L` |
