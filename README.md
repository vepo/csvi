# CSV Viewer
A CSV Viewer written in C.

For learning purpose.

Trying in Go too: https://github.com/vepo/csv-viewer-go/

# Convetions

For every function signature, follow the order:
1. `width  → x`
2. `height → y`

> :warning: Except for CSV purpose! In CSV we navigate on lines.

## Build process
To undestand a little more about the building process: http://blog.fourthbit.com/2013/06/18/creating-an-open-source-program-in-c-with-autotools-part-1-of-2/

## Dependencies

```bash
sudo apt update
sudo apt install gcc
sudo apt install libtool
sudo apt install make
sudo apt install check
sudo apt install pkg-config
sudo apt install libncurses5-dev
```

## Code Coverage

```bash
./conf-clean
./conf-gen
./configure --enable-coverage
make
make check
lcov -c --directory tests --output-file coverage.info
genhtml coverage.info --output-directory coverage-report
```