#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "csv-reader.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "No file to read...");
        exit(1);
    }

    csv_reader_read_file(argv[1]);
    return 0;
}