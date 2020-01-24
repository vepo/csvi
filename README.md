# CSV Viewer

## Build process
To undestand a little more about the building process: http://blog.fourthbit.com/2013/06/18/creating-an-open-source-program-in-c-with-autotools-part-1-of-2/

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

