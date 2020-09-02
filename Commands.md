# Commands

## Types

The command can be of the type:

* **Navigation**: Change the view state.
* **Mode**: Change the edition mode.
* **Command**: On edition mode command, apply commands do the editor.

## Command List

| Command | Type | Description |
|---------|------|-------------|
| Key Up | Navigation | Go one cell up |
| Key Down | Navigation | Go one cell down |
| Key Left | Navigation | Go one cell left |
| Key Right | Navigation | Go one cell Right |
| Page Up | Navigation | Go one page up, a page is defined by the size of the screen  |
| Page Down | Navigation | Go one page down, a page is defined by the size of the screen  |
| Home | Navigation | Go one page left, a page is defined by the size of the screen  |
| End | Navigation | Go one page right, a page is defined by the size of the screen  |
| Esc | Mode | Access the command interface  |
| ":q" | Command | Exit |
| ":[0-9]" | Command | Go to line |
| ":c[0-9]" | Command | Go to column |
| ":[0-9]x[0,9]" | Command | Go to cell, where the column is the first argument and the line the second. |
| ":0" | Command | Go to the first line | 
| ":-1" | Command | Go to the last line |
| ":c0" | Command | Go to the first column |
| ":c-1" | Command | Go to the last column |